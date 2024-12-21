#include "stream_task.hh"

namespace coralmicro {


    void test_msgpack() {
        // Allocate buffer on stack
        uint8_t buffer[256];
        struct umsgpack_packer_buf* packer = (struct umsgpack_packer_buf*)buffer;
        
        // Initialize packer
        umsgpack_packer_init(packer, sizeof(buffer));
        
        // Test packing various types
        printf("Testing MessagePack packing...\n");
        
        // Pack a map with different types
        umsgpack_pack_map(packer, 4);
        
        // Test integer
        umsgpack_pack_str(packer, "int", 3);
        umsgpack_pack_int32(packer, 12345);
        
        // Test float
        umsgpack_pack_str(packer, "float", 5);
        umsgpack_pack_float(packer, 3.14159f);
        
        // Test string
        umsgpack_pack_str(packer, "str", 3);
        umsgpack_pack_str(packer, "Hello M7!", 9);
        
        // Test array
        umsgpack_pack_str(packer, "array", 5);
        umsgpack_pack_array(packer, 3);
        umsgpack_pack_int(packer, 1);
        umsgpack_pack_int(packer, 2);
        umsgpack_pack_int(packer, 3);
        
        // Print result size
        printf("Packed size: %d bytes\n", umsgpack_get_length(packer));
        
        // Print hex dump of packed data
        printf("Packed data:\n");
        for(unsigned int i = 0; i < umsgpack_get_length(packer); i++) {
            printf("%02X ", packer->data[i]);
            if((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
    }


    void stream_task(void* parameters) {
        (void)parameters;

        test_msgpack();

        vTaskSuspend(nullptr);
        
    }

}
