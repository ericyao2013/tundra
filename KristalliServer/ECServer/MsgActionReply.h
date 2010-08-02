#pragma once

#include "clb/Network/DataDeserializer.h"
#include "clb/Network/DataSerializer.h"

struct MsgActionReply
{
	MsgActionReply()
	{
		InitToDefault();
	}

	MsgActionReply(const char *data, size_t numBytes)
	{
		InitToDefault();
		DataDeserializer dd(data, numBytes);
		DeserializeFrom(dd);
	}

	void InitToDefault()
	{
		reliable = true;
		inOrder = false;
		priority = 100;
	}

	static inline u32 MessageID() { return 115; }
	static inline const char *Name() { return "ActionReply"; }

	bool reliable;
	bool inOrder;
	u32 priority;

	u8 success;
	u32 actionID;
	u32 entityID;
	u8 entityUUID[16];

	inline size_t Size() const
	{
		return 1 + 4 + 4 + 16*1;
	}

	inline void SerializeTo(DataSerializer &dst) const
	{
		dst.Add<u8>(success);
		dst.Add<u32>(actionID);
		dst.Add<u32>(entityID);
		dst.AddArray<u8>(entityUUID, 16);
	}

	inline void DeserializeFrom(DataDeserializer &src)
	{
		success = src.Read<u8>();
		actionID = src.Read<u32>();
		entityID = src.Read<u32>();
		src.ReadArray<u8>(entityUUID, 16);
	}

};

