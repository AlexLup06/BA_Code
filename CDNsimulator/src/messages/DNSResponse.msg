import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class DNSResponse extends FieldsChunk {
	int serverId;	// if -1 then it is origin server because no surrogate has object in cache
	int objectId;
}