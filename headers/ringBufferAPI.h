//
// Created by markd on 2026-02-22.
//

#ifndef LIQUIDANALYTICSTECHNICAL_RINGBUFFERAPI_H
#define LIQUIDANALYTICSTECHNICAL_RINGBUFFERAPI_H

/*
 * Struct for the nodes in the buffer
 * Will store the message and pointer to next node
 */
typedef struct BufferNode {
    char *message;
    struct BufferNode *next;
} BufferNode;

/*
 * Struct for the ring buffer
 * Will store pointers to the read and write nodes on the buffer
 * Write will be pointed at the last node to be written
 * Read will be pointed at the next node to be read
 */
typedef struct RingBuffer {
    BufferNode *read;
    BufferNode *write;
    int bufferSize;
} RingBuffer;

RingBuffer* initBuffer();
/*
 * Function will write to the buffer by adding a node and writing the message to that node
 * Write pointer will be changed to most recent node
 * Ring buffer size gets increased
 * @param ringBuffer | The ring buffer that gets the new node
 * @param message | Text message from sensors
 */
void bufferWrite(RingBuffer *ringBuffer, char *message);

/*
 * Function will read from the buffer
 * Read pointer will be changed to next oldest node
 * Ring buffer size gets decreased
 * Will free bufferNode after reading
 * @param ringBuffer | The ring buffer that gets read
 * return message in bufferNode
 */
char *bufferRead(RingBuffer *ringBuffer);

/*
 * Function will free the contents of the bufferNode
 * @param node | the node being freed
 */
void freeBufferNode(BufferNode *node);

/*
 * Function will itteratively free the RingBuffer including all data stored in it
 * @param buffer | the ringBuffer being freed
 */
void freeRingBuffer(RingBuffer **buffer);

#endif //LIQUIDANALYTICSTECHNICAL_RINGBUFFERAPI_H
