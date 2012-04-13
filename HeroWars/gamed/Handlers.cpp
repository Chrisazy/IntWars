#include "PacketHandler.h"

bool PacketHandler::handleNull(HANDLE_ARGS)
{
	return true;
}

bool PacketHandler::handleKeyCheck(ENetPeer *peer, ENetPacket *packet)
{
	KeyCheck *keyCheck = (KeyCheck*)packet->data;

	uint8 test2[8] = {0x74, 0xF1, 0x18, 0x00, 0xF2, 0xB3, 0x67, 0x77};
	uint8 test[8] = {0xed, 0x8c, 0x4c, 0x74, 0x60, 0x99, 0x78, 0x7a};
	uint8 skeleton[8] = {0xed, 0x8c, 0xd9, 0x74, 0x62, 0xad, 0x4e, 0x56};

	uint64 userId = _blowfish->Decrypt(keyCheck->checkId);
	if(userId == keyCheck->userId)
	{
		PDEBUG_LOG_LINE(Log::getMainInstance()," User got the same key as i do, go on!\n");
		peerInfo(peer)->keyChecked = true;
		peerInfo(peer)->userId = userId;
	}
	else
	{
		Log::getMainInstance()->errorLine(" WRONG KEY, GTFO!!!\n");
		return false;
	}

	keyCheck->checkId = _blowfish->Encrypt(userId);


	//Send response as this is correct (OFC DO SOME ID CHECKS HERE!!!)
	KeyCheck response;
	response.userId = keyCheck->userId;
	response.netId = 0;
	//response.checkId = 5;
	memcpy(response.pCheckId(), skeleton, 8);

	uint8 *key = _blowfish->getKey();
	memcpy(&response.partialKey[0], &key[1], 3);
	
	return sendPacket(peer, reinterpret_cast<uint8*>(&response), sizeof(KeyCheck), 0); //channel 0 check
}

bool PacketHandler::handleGameNumber(ENetPeer *peer, ENetPacket *packet)
{
	WorldSendGameNumber world;
	world.gameId = 359250843;
	memcpy(world.data, "IMxHot", 6);

	return sendPacket(peer, reinterpret_cast<uint8*>(&world), sizeof(WorldSendGameNumber), 3); //channel 3 check
}

bool PacketHandler::handleSynch(ENetPeer *peer, ENetPacket *packet)
{
	SynchVersion *version = reinterpret_cast<SynchVersion*>(packet->data);
	Log::getMainInstance()->writeLine("Client version: %s\n", version->version);

	SynchVersionAns answer;
	answer.mapId = 1;
	//exhaust = 0x08A8BAE4, Cleanse = 0x064D2094, flash = 0x06496EA8
	answer.players[0].userId = peerInfo(peer)->userId;
	answer.players[0].skill1 = 0x08A8BAE4;
	answer.players[0].skill2 = 0x064D2094;

	sendPacket(peer, reinterpret_cast<uint8*>(&answer), sizeof(SynchVersionAns), 3);

	return true;
}

bool PacketHandler::handleMap(ENetPeer *peer, ENetPacket *packet)
{
	uint8 spawnStart[] = {0x67, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x17, 0x87, 0xb2, 0x22, \
		0x71, 0xae, 0x33, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0xb0, 0x4b, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x71, 0xae, 0x33, 0x01, 0x00, 0x00, 0x00, 0x00, \
		0xed, 0x8c, 0xd9, 0x74, 0x62, 0xad, 0x4e, 0x56, 0xfe, 0xff, 0xff, 0xff, 0x3c, 0xf1, 0x18, 0x00, \
		0xb2, 0x0c, 0x6a, 0x00, 0xf0, 0xf5, 0x18, 0x00, 0xed, 0x8c, 0xd9, 0x74, 0xf2, 0xad, 0x4e, 0x56, \
		0xfe, 0xff, 0xff, 0xff, 0x58, 0x4c, 0xd9, 0x74, 0xa0, 0x0c, 0x6a, 0x00, 0x38, 0xe0, 0xb3, 0x05, \
		0x70, 0x76, 0x64, 0x03, 0x8f, 0x0e, 0x5e, 0x67, 0x8f, 0x0e, 0x5e, 0x67, 0x0c, 0x27, 0x3e, 0x75, \
		0x9d, 0x6f, 0x3b, 0x04, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x70, 0x76, 0x64, 0x03, \
		0x9e, 0xbc, 0x5b, 0x00, 0x9c, 0xf7, 0x18, 0x00, 0xdd, 0xbd, 0x5b, 0x00, 0x20, 0x74, 0x6f, 0x20, \
		0xf1, 0x1a, 0x00, 0x00, 0x6d, 0x61, 0x70, 0x70, 0x69, 0x6e, 0x67, 0x73, 0x20, 0x3a, 0x2c, 0x20, \
		0x30, 0x2c, 0x20, 0x31, 0x00, 0x39, 0x2c, 0x20, 0x74, 0x6f, 0x20, 0x65, 0x6e, 0x65, 0x74, 0x20, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x02, 0xc7, 0xda, 0x74, 0x50, 0xf2, 0x18, 0x00, 0xb8, 0x6d, 0x5f, 0x03, 0xa8, 0x15, 0x26, 0x03, \
		0x2f, 0x2e, 0xd9, 0x74, 0x00, 0xf2, 0x18, 0x00, 0xb4, 0xf2, 0x18, 0x00, 0x64, 0x1a, 0xda, 0x74, \
		0x0a, 0x00, 0x00, 0x00, 0x91, 0x1a, 0xda, 0x74, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x1b, 0xe2, 0x74, \
		0x63, 0x20, 0xda, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x02, \
		0x00, 0x00, 0xc4, 0x02, 0xe8, 0xff, 0xc6, 0x02, 0x00, 0x00, 0x00, 0x00, 0xe8, 0xff, 0xc6, 0x02, \
		0x74, 0xf2, 0x18, 0x00, 0xf2, 0xb3, 0xe8, 0x77, 0xe8, 0xff, 0xc6, 0x02, 0xe0, 0xff, 0xc6, 0x02, \
		0x00, 0x00, 0xc4, 0x02, 0x75, 0xb3, 0xe8, 0x77, 0xe8, 0xef, 0xc6, 0x02, 0x00, 0x00, 0xc4, 0x02, \
		0xe0, 0xef, 0xc6, 0x02, 0xa8, 0xf2, 0x18, 0x00, 0x00, 0xe2, 0xc6, 0x02, 0x00, 0x00, 0xc4, 0x02, \
		0x00, 0x00, 0x00, 0x00, 0xe0, 0x19, 0x00, 0x00, 0xbc, 0x03, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x02, \
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	SpawnPacket *spawnName = SpawnPacket::create(PKT_LoadName, peerInfo(peer)->name, peerInfo(peer)->nameLen);
	spawnName->userId = peerInfo(peer)->userId;

	SpawnPacket *spawnType = SpawnPacket::create(PKT_LoadType,  peerInfo(peer)->type, peerInfo(peer)->typeLen);
	spawnType->userId = peerInfo(peer)->userId;

	bool p1 = sendPacket(peer, reinterpret_cast<uint8*>(spawnStart), sizeof(spawnStart), 6);
	bool p2 = sendPacket(peer, reinterpret_cast<uint8*>(spawnName), spawnName->getPacketLength(), 6);
	bool p3 = sendPacket(peer, reinterpret_cast<uint8*>(spawnType), spawnType->getPacketLength(), 6);

	return (p1 && p2 && p3);

}

bool PacketHandler::handleSpawn(ENetPeer *peer, ENetPacket *packet)
{
	uint8 bounds[] = {0x2c, 0x19, 0x00, 0x00, 0x40, 0x99, 0x14, 0x00, 0x00, 0x99, 0x14, 0x00, 0x00, 0x7f, 0x14, 0x00,\
		0x00, 0x7f, 0x14, 0x00, 0x00, 0x7f, 0x14, 0x00, 0x00, 0x7f, 0x14, 0x00, 0x00, 0x7f, 0x14, 0x00,\
		0x00, 0xe1, 0x14, 0x00, 0x00, 0xe1, 0x14, 0x00, 0x00, 0xe1, 0x14, 0x00, 0x00, 0x99, 0x14, 0x00,\
		0x00, 0x99, 0x14, 0x00, 0x00, 0xa9, 0x14, 0x00, 0x00, 0xa9, 0x14, 0x00, 0x00, 0xa9, 0x14, 0x00,\
		0x00, 0xa9, 0x14, 0x00, 0x00, 0xa9, 0x14, 0x00, 0x00, 0xc5, 0x14, 0x00, 0x00, 0xc5, 0x14, 0x00,\
		0x00, 0xc5, 0x14, 0x00, 0x00, 0xc5, 0x14, 0x00, 0x00, 0xa9, 0x14, 0x00, 0x00, 0xc5, 0x14, 0x00,\
		0x00, 0xa9, 0x14, 0x00, 0x00, 0xc5, 0x14, 0x00, 0x00, 0xa9, 0x14, 0x00, 0x00, 0xc5, 0x14, 0x00,\
		0x00, 0xa9, 0x14, 0x00, 0x00, 0xc5, 0x14, 0x00, 0x00, 0xc5, 0x14, 0x00, 0x00, 0x95, 0x86, 0x5e,\
		0x06, 0xa8, 0x6e, 0x49, 0x06, 0x64, 0x37, 0x00, 0x00, 0x01, 0x44, 0x36, 0x00, 0x00, 0x02, 0x41,\
		0x36, 0x00, 0x00, 0x01, 0x52, 0x37, 0x00, 0x00, 0x04, 0x71, 0x36, 0x00, 0x00, 0x01, 0x43, 0x36,\
		0x00, 0x00, 0x03, 0x42, 0x36, 0x00, 0x00, 0x03, 0x41, 0x37, 0x00, 0x00, 0x01, 0x64, 0x36, 0x00,\
		0x00, 0x01, 0x92, 0x36, 0x00, 0x00, 0x01, 0x52, 0x36, 0x00, 0x00, 0x04, 0x43, 0x37, 0x00, 0x00,\
		0x03, 0x61, 0x36, 0x00, 0x00, 0x01, 0x82, 0x36, 0x00, 0x00, 0x03, 0x62, 0x36, 0x00, 0x00, 0x01,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x1e};

	uint8 turrets1[] = {0xff, 0x06, 0x4f, 0xa6, 0x01, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72,\
		0x72, 0x65, 0x74, 0x5f, 0x54, 0x31, 0x5f, 0x52, 0x5f, 0x30, 0x33, 0x5f, 0x41, 0x00, 0x74, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5,\
		0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04,\
		0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00,\
		0x80, 0x01, 0xff, 0x01, 0x4a, 0x02, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74,\
		0x5f, 0x54, 0x31, 0x5f, 0x52, 0x5f, 0x30, 0x32, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76,\
		0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00,\
		0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff,\
		0x01, 0x4a, 0x03, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x31,\
		0x5f, 0x43, 0x5f, 0x30, 0x37, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59,\
		0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00,\
		0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x04,\
		0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x52, 0x5f,\
		0x30, 0x33, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02,\
		0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9,\
		0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8,\
		0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x05, 0x00, 0x00, 0x40,\
		0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x52, 0x5f, 0x30, 0x32, 0x5f,\
		0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03,\
		0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2,\
		0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38,\
		0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x06, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75,\
		0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x52, 0x5f, 0x30, 0x31, 0x5f, 0x41, 0x00, 0x74,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00,\
		0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74,\
		0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00,\
		0x00, 0x80, 0x01, 0x12, 0x00, 0x00, 0x00, 0x00};

	uint8 turrets2[] = {0xff, 0x06, 0x4f, 0xa6, 0x07, 0x00, 0x00, 0x40, 0x07, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72,\
		0x72, 0x65, 0x74, 0x5f, 0x54, 0x31, 0x5f, 0x43, 0x5f, 0x30, 0x35, 0x5f, 0x41, 0x00, 0x74, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5,\
		0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04,\
		0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00,\
		0x80, 0x01, 0xff, 0x01, 0x4a, 0x08, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74,\
		0x5f, 0x54, 0x31, 0x5f, 0x43, 0x5f, 0x30, 0x34, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76,\
		0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00,\
		0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff,\
		0x01, 0x4a, 0x09, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x31,\
		0x5f, 0x43, 0x5f, 0x30, 0x33, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59,\
		0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00,\
		0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x0a,\
		0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x31, 0x5f, 0x43, 0x5f,\
		0x30, 0x31, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02,\
		0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9,\
		0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8,\
		0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x0b, 0x00, 0x00, 0x40,\
		0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x31, 0x5f, 0x43, 0x5f, 0x30, 0x32, 0x5f,\
		0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03,\
		0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2,\
		0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38,\
		0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x0c, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75,\
		0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x43, 0x5f, 0x30, 0x35, 0x5f, 0x41, 0x00, 0x74,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00,\
		0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74,\
		0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00,\
		0x00, 0x80, 0x01};

	uint8 turrets3[] = {0xff, 0x06, 0x4f, 0xa6, 0x0d, 0x00, 0x00, 0x40, 0x0d, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72,\
		0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x43, 0x5f, 0x30, 0x34, 0x5f, 0x41, 0x00, 0x74, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5,\
		0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04,\
		0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00,\
		0x80, 0x01, 0xff, 0x01, 0x4a, 0x0e, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74,\
		0x5f, 0x54, 0x32, 0x5f, 0x43, 0x5f, 0x30, 0x33, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76,\
		0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00,\
		0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff,\
		0x01, 0x4a, 0x0f, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32,\
		0x5f, 0x43, 0x5f, 0x30, 0x31, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59,\
		0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00,\
		0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x10,\
		0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x43, 0x5f,\
		0x30, 0x32, 0x5f, 0x41, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02,\
		0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9,\
		0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8,\
		0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x11, 0x00, 0x00, 0x40,\
		0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x4f, 0x72, 0x64, 0x65, 0x72, 0x54, 0x75, 0x72,\
		0x72, 0x65, 0x74, 0x53, 0x68, 0x72, 0x69, 0x6e, 0x65, 0x5f, 0x41, 0x00, 0x02, 0x00, 0x00, 0x03,\
		0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2,\
		0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38,\
		0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x12, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75,\
		0x72, 0x72, 0x65, 0x74, 0x5f, 0x43, 0x68, 0x61, 0x6f, 0x73, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74,\
		0x53, 0x68, 0x72, 0x69, 0x6e, 0x65, 0x5f, 0x41, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00,\
		0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74,\
		0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00,\
		0x00, 0x80, 0x01};

uint8 turrets4[] = {0xff, 0x07, 0x4f, 0xa6, 0x13, 0x00, 0x00, 0x40, 0x13, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72,\
		0x72, 0x65, 0x74, 0x5f, 0x54, 0x31, 0x5f, 0x4c, 0x5f, 0x30, 0x33, 0x5f, 0x41, 0x00, 0x74, 0x53,\
		0x68, 0x72, 0x69, 0x6e, 0x65, 0x5f, 0x41, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5,\
		0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04,\
		0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00,\
		0x80, 0x01, 0xff, 0x01, 0x4a, 0x14, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74,\
		0x5f, 0x54, 0x31, 0x5f, 0x4c, 0x5f, 0x30, 0x32, 0x5f, 0x41, 0x00, 0x74, 0x53, 0x68, 0x72, 0x69,\
		0x6e, 0x65, 0x5f, 0x41, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76,\
		0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00,\
		0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff,\
		0x01, 0x4a, 0x15, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x31,\
		0x5f, 0x43, 0x5f, 0x30, 0x36, 0x5f, 0x41, 0x00, 0x74, 0x53, 0x68, 0x72, 0x69, 0x6e, 0x65, 0x5f,\
		0x41, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59,\
		0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00,\
		0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x16,\
		0x00, 0x00, 0x40, 0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x4c, 0x5f,\
		0x30, 0x33, 0x5f, 0x41, 0x00, 0x74, 0x53, 0x68, 0x72, 0x69, 0x6e, 0x65, 0x5f, 0x41, 0x00, 0x02,\
		0x00, 0x00, 0x03, 0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9,\
		0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8,\
		0x18, 0x00, 0x38, 0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x17, 0x00, 0x00, 0x40,\
		0x40, 0x54, 0x75, 0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x4c, 0x5f, 0x30, 0x32, 0x5f,\
		0x41, 0x00, 0x74, 0x53, 0x68, 0x72, 0x69, 0x6e, 0x65, 0x5f, 0x41, 0x00, 0x02, 0x00, 0x00, 0x03,\
		0x00, 0x1f, 0x00, 0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2,\
		0xb9, 0xa8, 0x74, 0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38,\
		0x14, 0x68, 0x00, 0x00, 0x80, 0x01, 0xff, 0x01, 0x4a, 0x18, 0x00, 0x00, 0x40, 0x40, 0x54, 0x75,\
		0x72, 0x72, 0x65, 0x74, 0x5f, 0x54, 0x32, 0x5f, 0x4c, 0x5f, 0x30, 0x31, 0x5f, 0x41, 0x00, 0x74,\
		0x53, 0x68, 0x72, 0x69, 0x6e, 0x65, 0x5f, 0x41, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x1f, 0x00,\
		0xc5, 0x16, 0x68, 0x76, 0xdd, 0x46, 0x59, 0x5d, 0xe2, 0xf9, 0x33, 0x77, 0xf2, 0xb9, 0xa8, 0x74,\
		0x04, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0xe8, 0xf8, 0x18, 0x00, 0x38, 0x14, 0x68, 0x00,\
		0x00, 0x80, 0x01, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00};

	HeroSpawnPacket *heroSpawn = HeroSpawnPacket::create(PKT_HeroSpawn,  peerInfo(peer)->name, peerInfo(peer)->nameLen, peerInfo(peer)->type, peerInfo(peer)->typeLen);

	sendPacket(peer, reinterpret_cast<uint8*>(heroSpawn), heroSpawn->getPacketLength(), 3);
	sendPacket(peer, reinterpret_cast<uint8*>(bounds), sizeof(bounds), 3);
	sendPacket(peer, reinterpret_cast<uint8*>(turrets1), sizeof(turrets1), 3);
	sendPacket(peer, reinterpret_cast<uint8*>(turrets2), sizeof(turrets2), 3);
	sendPacket(peer, reinterpret_cast<uint8*>(turrets3), sizeof(turrets3), 3);
	sendPacket(peer, reinterpret_cast<uint8*>(turrets4), sizeof(turrets4), 3);

	
	return true;
}

bool PacketHandler::handleInit(HANDLE_ARGS)
{
	uint8 resp[] = {0xff, 0x18, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x24, 0x12, 0xca, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x50, 0xc0, 0x01, 0x00, 0x00, 0x40, 0x00, 0x01, 0x01, 0x00, 0x43, 0xac, 0xd4, 0x0c, 0x00, 0x00,\
		0x00, 0x00, 0xcd, 0x0c, 0xf0, 0x43, 0x01, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00,\
		0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd2, 0x43, 0x02, 0x00, 0x00, 0x40,\
		0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x70, 0x44, 0x03, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x43, 0xac, 0xd4, 0x0c,\
		0x00, 0x00, 0x00, 0x00, 0xcd, 0x0c, 0xf0, 0x43, 0x04, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01,\
		0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd2, 0x43, 0x05, 0x00,\
		0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x70, 0x44, 0x06, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x43, 0xac,\
		0xd4, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xcd, 0x0c, 0xf0, 0x43, 0x07, 0x00, 0x00, 0x40, 0x53, 0x01,\
		0x00, 0x01, 0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd2, 0x43,\
		0x08, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x00, 0x70, 0x44, 0x09, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00,\
		0x43, 0xac, 0xd4, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x81, 0x0e, 0x45, 0x0a, 0x00, 0x00, 0x40,\
		0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x43, 0xac, 0xd4, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x81,\
		0x0e, 0x45, 0x0b, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x43, 0xac, 0xd4, 0x0c,\
		0x00, 0x00, 0x00, 0x00, 0xcd, 0x0c, 0xf0, 0x43, 0x0c, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01,\
		0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd2, 0x43, 0x0d, 0x00,\
		0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x70, 0x44, 0x0e, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x43, 0xac,\
		0xd4, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x81, 0x0e, 0x45, 0x0f, 0x00, 0x00, 0x40, 0x53, 0x01,\
		0x00, 0x01, 0x01, 0x00, 0x43, 0xac, 0xd4, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x81, 0x0e, 0x45,\
		0x10, 0x00, 0x00, 0x40, 0x53, 0x02, 0x00, 0x01, 0x01, 0x00, 0x43, 0xac, 0xd4, 0x0c, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0x50, 0xc3, 0x46, 0x12, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00,\
		0x43, 0xac, 0xd4, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xcd, 0x0c, 0xf0, 0x43, 0x13, 0x00, 0x00, 0x40,\
		0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0xd2, 0x43, 0x14, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x03, 0xa9, 0x21, 0x04,\
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x44, 0x15, 0x00, 0x00, 0x40, 0x53, 0x01, 0x00, 0x01,\
		0x01, 0x00, 0x43, 0xac, 0xd4, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xcd, 0x0c, 0xf0, 0x43, 0x16, 0x00,\
		0x00, 0x40, 0x53, 0x01, 0x00, 0x01, 0x01, 0x00, 0x03, 0xa9, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0xd2, 0x43, 0x17, 0x00, 0x00, 0x40};

	sendPacket(peer, reinterpret_cast<uint8*>(resp), sizeof(resp), 3);
	//sendPacket(peer, reinterpret_cast<uint8*>(fog), sizeof(fog), 4);
	
	return true;

}

bool PacketHandler::handleAttentionPing(ENetPeer *peer, ENetPacket *packet)
{
	AttentionPing *ping = reinterpret_cast<AttentionPing*>(packet->data);
	AttentionPingAns response(ping);

	Log::getMainInstance()->writeLine("Plong x: %f, y: %f, z: %f, type: %i\n", ping->x, ping->y, ping->z, ping->type);

	return broadcastPacket(reinterpret_cast<uint8*>(&response), sizeof(AttentionPing), 3);
}

bool PacketHandler::handleView(ENetPeer *peer, ENetPacket *packet)
{
	ViewReq *request = reinterpret_cast<ViewReq*>(packet->data);

	Log::getMainInstance()->writeLine("View (%i), x:%f, y:%f, zoom: %f\n", request->requestNo, request->x, request->y, request->zoom);

	ViewAns answer;
	answer.requestNo = request->requestNo;
	return sendPacket(peer, reinterpret_cast<uint8*>(&answer), sizeof(ViewAns), 3, UNRELIABLE);
}

bool PacketHandler::handleMove(ENetPeer *peer, ENetPacket *packet)
{
	MoveReq *request = reinterpret_cast<MoveReq*>(packet->data);

	Log::getMainInstance()->writeLine("Move to: x(left->right):%f, y(height):%f, z(bot->top): %f\n", request->x1, request->y1, request->z1);
	return true;
}

bool PacketHandler::handleLoadPing(ENetPeer *peer, ENetPacket *packet)
{
	PingLoadInfo *loadInfo = reinterpret_cast<PingLoadInfo*>(packet->data);

	PingLoadInfo response;
	memcpy(&response, packet->data, sizeof(PingLoadInfo));
	response.header.cmd = PKT_S2C_Ping_Load_Info;
	response.userId = peerInfo(peer)->userId;


	Log::getMainInstance()->writeLine("Loading: loaded: %f, ping: %f, %i, %f\n", loadInfo->loaded, loadInfo->ping, loadInfo->unk4, loadInfo->f3);
	return broadcastPacket(reinterpret_cast<uint8*>(&response), sizeof(PingLoadInfo), 4, UNRELIABLE);
}

bool PacketHandler::handleQueryStatus(HANDLE_ARGS)
{
	QueryStatus response;
	return sendPacket(peer, reinterpret_cast<uint8*>(&response), sizeof(QueryStatus), 3);
}

bool PacketHandler::handleChatBoxMessage(HANDLE_ARGS)
{
	ChatBoxMessage* message = reinterpret_cast<ChatBoxMessage*>(packet->data);

	switch(message->cmd)
	{
	case CMT_ALL:
	//!TODO make a player class and foreach player in game send the message
		return sendPacket(peer,packet->data,packet->dataLength,CHL_COMMUNICATION);
		break;
	case CMT_TEAM:
	//!TODO make a team class and foreach player in the team send the message
		return sendPacket(peer,packet->data,packet->dataLength,CHL_COMMUNICATION);
		break;
	default:
		Log::getMainInstance()->errorLine("Unknow ChatMessageType");
		break;
	}
	return false;
}