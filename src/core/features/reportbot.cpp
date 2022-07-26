#include "../../includes.hpp"
#include "features.hpp"

std::vector<std::uint64_t> Features::ReportBot::getXuidsOfCandidatesToBeReported() {
  std::vector<std::uint64_t> xuids;

  for (int i = 1; i <= Interfaces::engine->GetMaxClients(); ++i) {
    const auto entity = (Player*)Interfaces::entityList->GetClientEntity(i);
    if (!entity || i == Globals::localPlayer->index())
      continue;

    if (!entity->isEnemy() && !CONFIGBOOL("Misc>Misc>ReportBot>TeamMate"))
      continue;

    player_info_t playerInfo;
    if (!Interfaces::engine->GetPlayerInfo(i, &playerInfo) || playerInfo.fakeplayer)
      continue;

    xuids.emplace_back(playerInfo.xuid);
  }

  return xuids;
}

std::string Features::ReportBot::generateReportString() {

  int flags = CONFIGINT("Misc>Misc>ReportBot>Report Flags");

  std::stringstream str;
  str << (flags & (int)HitBoxes::HEAD ? "textabuse, " : "")
                   << (flags & (int)HitBoxes::NECK ? "grief, " : "")
                   << (flags & (int)HitBoxes::CHEST ? "wallhack, " : "")
                   << (flags & (int)HitBoxes::STOMACH ? "aimbot, " : "")
                   << (flags & (int)HitBoxes::PELVIS ? "speedhack, "
                                                           : "");
  return str.str();
}

bool Features::ReportBot::isPlayerReported(std::uint64_t xuid) {
  return std::ranges::find(std::as_const(reportedPlayers), xuid) !=
         reportedPlayers.cend();
}

void Features::ReportBot::createMove(CUserCmd *cmd) {
  if (!Interfaces::engine->IsInGame() || !Globals::localPlayer || !Interfaces::globals) {
    return;
  }
  if (!CONFIGBOOL("Misc>Misc>ReportBot")) {
    reportedPlayers.clear();
    return;
  }
  static auto lastReportTime = 0.0f;

  if (lastReportTime + 1 > Interfaces::globals->realtime)
    return;

  for (const auto &xuid : getXuidsOfCandidatesToBeReported()) {
    if (isPlayerReported(xuid))
      continue;

    if (const auto report = generateReportString(); !report.empty()) {
      Offsets::submitReport(nullptr, std::to_string(xuid).c_str(), report.c_str());
      lastReportTime = Interfaces::globals->realtime;
      reportedPlayers.push_back(xuid);
      return;
    }
  }

  if (CONFIGBOOL("Misc>Misc>ReportBot>InfiniReport")) {
    reportedPlayers.clear();
  }
}