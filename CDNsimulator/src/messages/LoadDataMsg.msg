import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class LoadDataMsg extends FieldsChunk {
	int load;
	int surrogateId;
}