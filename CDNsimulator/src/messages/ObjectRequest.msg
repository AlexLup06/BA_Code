import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class ObjectRequest extends FieldsChunk {
	int objectId;	// id of the object the surrogate needs
}