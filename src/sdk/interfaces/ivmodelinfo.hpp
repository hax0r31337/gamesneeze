// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/public/studio.h

#define BONE_USED_BY_ANYTHING		0x000FFF00
#define BONE_USED_BY_HITBOX			0x00000100	// bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT		0x00000200	// bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK	0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0	0x00000400	// bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1	0x00000800	
#define BONE_USED_BY_VERTEX_LOD2	0x00001000  
#define BONE_USED_BY_VERTEX_LOD3	0x00002000
#define BONE_USED_BY_VERTEX_LOD4	0x00004000
#define BONE_USED_BY_VERTEX_LOD5	0x00008000
#define BONE_USED_BY_VERTEX_LOD6	0x00010000
#define BONE_USED_BY_VERTEX_LOD7	0x00020000
#define BONE_USED_BY_BONE_MERGE		0x00040000	// bone is available for bone merge to occur against it
#define BONE_ALWAYS_SETUP			0x00080000

#define MAXSTUDIOBONES		256

struct mstudiobone_t {
	int					sznameindex;
	inline char*        pszName( void ) const { return ((char* )this) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

	// default values
	Vector				pos;
	Quaternion			quat;
	RadianEuler			rot;
	// compression scale
	Vector				posscale;
	Vector				rotscale;

	matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void*        pProcedure( ) const { if (procindex == 0) return 0; else return  (void* )(((char*)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char*        pszSurfaceProp( void ) const { return ((char* )this) + surfacepropidx; }
	inline int			GetSurfaceProp( void ) const { return surfacepropLookup; }

	int					contents;		// See BSPFlags.h for the contents flags
	int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file
	int					unused[7];		// remove as appropriate
};

struct mstudiobbox_t {
	int bone;
	int group;
	Vector bbmin;
	Vector bbmax;
	int hitboxnameindex;
	Vector rotation;
	float radius;
	int pad2[4];

	char* pszHitboxName()
	{
		if (hitboxnameindex == 0)
			return nullptr;

		return ((char* ) this) + hitboxnameindex;
	}
};

struct mstudiohitboxset_t {
	int sznameindex;

	inline char* pszName() const
	{
		return ((char* ) this) + sznameindex;
	}

	int numhitboxes;
	int hitboxindex;

	inline mstudiobbox_t* pHitbox(int i) const
	{
		return (mstudiobbox_t* ) (((unsigned char* ) this) + hitboxindex) + i;
	};
};

struct studiohdr_t {
	int id;
	int version;
	int checksum;        // this has to be the same in the phy and vtx files to load!
	char name[64];
	int length;

	Vector eyeposition;    // ideal eye position
	Vector illumposition;    // illumination center
	Vector hull_min;        // ideal movement hull size
	Vector hull_max;
	Vector view_bbmin;        // clipping bounding box
	Vector view_bbmax;

	int flags;
	int numbones;            // bones
	int boneindex;
	inline mstudiobone_t* pBone(int i) const
	{
		Assert(i >= 0 && i < numbones);
		return (mstudiobone_t* )(((unsigned char*  ) this) + boneindex ) + i;
	};

	int RemapSeqBone(int iSequence, int iLocalBone) const;    // maps local sequence bone to global bone
	int RemapAnimBone(int iAnim, int iLocalBone) const;        // maps local animations bone to global bone
	int numbonecontrollers;        // bone controllers
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;

	// Look up hitbox set by index
	mstudiohitboxset_t* pHitboxSet(int i) const
	{
		(i >= 0 && i < numhitboxsets);
		return (mstudiohitboxset_t*  )(((unsigned char*  ) this ) +hitboxsetindex ) +i;
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t* pHitbox(int i, int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return nullptr;

		return s->pHitbox(i);
	};

	// Calls through to set to get hitbox count for set
	inline int iHitboxCount(int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return 0;

		return s->numhitboxes;
	};

	// file local animations? and sequences
	//private:
	int numlocalanim;            // animations/poses
	int localanimindex;        // animation descriptions
	int numlocalseq;                // sequences
	int localseqindex;

	//public:
	bool SequencesAvailable() const;

	int GetNumSeq() const;

	int iRelativeAnim(int baseseq, int relanim) const;    // maps seq local anim reference to global anim index
	int iRelativeSeq(int baseseq, int relseq) const;        // maps seq local seq reference to global seq index

	//private:
	mutable int activitylistversion;    // initialization flag - have the sequences been indexed?
	mutable int eventsindexed;

	//public:
	int GetSequenceActivity(int iSequence);

	void SetSequenceActivity(int iSequence, int iActivity);

	int GetActivityListVersion();

	void SetActivityListVersion(int version) const;

	int GetEventListVersion();

	void SetEventListVersion(int version);

	// raw textures
	int numtextures;
	int textureindex;

	// raw textures search paths
	int numcdtextures;
	int cdtextureindex;

	inline char* pCdtexture(int i) const
	{
		return (((char* ) this) +* ((int* ) (((unsigned char* ) this) +cdtextureindex) + i));
	};

	// replaceable textures tables
	int numskinref;
	int numskinfamilies;
	int skinindex;

	inline short* pSkinref(int i) const
	{
		return (short* ) (((unsigned char* ) this) +skinindex) +i;
	};
	int numbodyparts;
	int bodypartindex;

	// queryable attachable points
	//private:
	int numlocalattachments;
	int localattachmentindex;

	//public:
	int GetNumAttachments() const;

	int GetAttachmentBone(int i);

	// used on my tools in hlmv, not persistant
	void SetAttachmentBone(int iAttachment, int iBone);

	// animation node to animation node transition graph
	//private:
	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;

	inline char* pszLocalNodeName(int iNode) const
	{
		(iNode >= 0 && iNode < numlocalnodes);
		return (((char* ) this) +* ((int* ) (((unsigned char* ) this) + localnodenameindex) + iNode));
	}

	inline unsigned char* pLocalTransition(int i) const
	{
		(i >= 0 && i < (numlocalnodes*  numlocalnodes));
		return (unsigned char*  )(((unsigned char* ) this) + localnodeindex) + i;
	};

	//public:
	int EntryNode(int iSequence);

	int ExitNode(int iSequence);

	char* pszNodeName(int iNode);

	int GetTransition(int iFrom, int iTo) const;

	int numflexdesc;
	int flexdescindex;
	int numflexcontrollers;
	int flexcontrollerindex;
	int numflexrules;
	int flexruleindex;
	int numikchains;
	int ikchainindex;
	int nummouths;
	int mouthindex;

	//private:
	int numlocalposeparameters;
	int localposeparamindex;

	//public:
	int GetNumPoseParameters() const;

	int GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int surfacepropindex;

	inline char* pszSurfaceProp() const
	{
		return ((char* ) this) + surfacepropindex;
	}

	// Key values
	int keyvalueindex;
	int keyvaluesize;

	inline const char* KeyValueText() const
	{
		return keyvaluesize != 0 ? ((char* ) this) + keyvalueindex : nullptr;
	}

	int numlocalikautoplaylocks;
	int localikautoplaylockindex;

	int GetNumIKAutoplayLocks() const;

	int CountAutoplaySequences() const;

	int CopyAutoplaySequences(unsigned short* pOut, int outCount) const;

	int GetAutoplayList(unsigned short* *pOut) const;

	// The collision model mass that jay wanted
	float mass;
	int contents;

	// external animations, models, etc.
	int numincludemodels;
	int includemodelindex;

	// implementation specific call to get a named model
	const studiohdr_t* FindModel(void* *cache, char const* modelname) const;

	// implementation specific back pointer to virtual data
	mutable void* virtualModel;
	//virtualmodel_t		GetVirtualModel() const;

	// for demand loaded animation blocks
	int szanimblocknameindex;

	inline char* pszAnimBlockName() const
	{
		return ((char* ) this) + szanimblocknameindex;
	}

	int numanimblocks;
	int animblockindex;
	mutable void* animblockModel;

	unsigned char* GetAnimBlock(int i) const;

	int bonetablebynameindex;

	inline const unsigned char* GetBoneTableSortedByName() const
	{
		return (unsigned char* ) this + bonetablebynameindex;
	}

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	void* pVertexBase;
	void* pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting
	// on static props
	unsigned char constdirectionallightdot;

	// set during load of mdl data to track* desired* lod configuration (not actual)
	// the* actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	unsigned char rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	unsigned char numAllowedRootLODs;
	unsigned char unused[1];
	int unused4; // zero out if version < 47
	int numflexcontrollerui;
	int flexcontrolleruiindex;
	int unused3[2];

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int studiohdr2index;

	// NOTE: No room to add stuff? Up the .mdl file format version
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int unused2[1];

	studiohdr_t() { }
private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t &vOther);

	friend struct virtualmodel_t;
};

class IVModelInfo {
public:
	int GetModelIndex(const char* Filename) {
		typedef int (*Fn)(void*, const char*);
		return getVirtualFunc<Fn>(this, 3)(this, Filename);
	}

	const char* GetModelName(const model_t* model) {
		typedef const char* (*Fn)(void*, const model_t*);
		return getVirtualFunc<Fn>(this, 4)(this, model);
	}

	studiohdr_t* GetStudioModel(const model_t* model) {
		typedef studiohdr_t* (*Fn)(void*, const model_t*);
		return getVirtualFunc<Fn>(this, 31)(this, model);
	}
};