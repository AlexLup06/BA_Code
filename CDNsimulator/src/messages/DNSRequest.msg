import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class DNSRequest extends FieldsChunk {
	int objectId;	// id of the object the surrogate needs
	int connId;	// the id of the connecttion of client and surrogate server which wants the object
	int surrogateId;
}