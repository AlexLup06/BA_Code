import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class MyObjectResponse extends FieldsChunk {
	int objectId;
	long size;
}