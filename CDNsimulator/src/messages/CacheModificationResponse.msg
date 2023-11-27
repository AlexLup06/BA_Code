import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class CacheModificationResponse extends FieldsChunk {
	int objectToDelete;	// id of object we are deleting
}
