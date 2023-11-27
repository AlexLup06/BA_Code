import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class CacheModificationRequest extends FieldsChunk {
	int type;	// 0: delete, 1: adding
	int objectId;	// id of object we are adding or deleting
	int surrogateIndex;
	int objectToRemove;
}