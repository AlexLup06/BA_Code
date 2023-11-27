import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class ServerResponse extends FieldsChunk {
	short statusCode;
	int requestedObjectId;
	long size;
}
