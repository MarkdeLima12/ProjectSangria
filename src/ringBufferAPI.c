//
// Created by markd on 2026-02-22.
//
#include "../headers/ringBufferAPI.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Copies the message provided to memory and returns it
 * @param message | the message to be copied
 */
static char *copyMessage(const char *message) {
    // Error Checks
    if (message == NULL) {
        return NULL;
    }

    // Variables + Init
    int len = strlen(message);
    char *copy = (char *)malloc(len + 1);

    if (copy == NULL) {
        return NULL;
    }

    // Memory copy and return memory
    memcpy(copy, message, len + 1);
    return copy;
}

RingBuffer* initBuffer() {
    RingBuffer *ringBuffer = (RingBuffer *)malloc(sizeof(RingBuffer));
    if (ringBuffer == NULL) {
        printf("Malloc Failed");
        return NULL;
    }
    ringBuffer->bufferSize = 0;
    ringBuffer->read = NULL;
    ringBuffer->write = NULL;
    return ringBuffer;
}

void bufferWrite(RingBuffer *ringBuffer, char *message) {
    // Error checks
    if (ringBuffer == NULL) {
        printf("RingBuffer NULL pointer\n");
        return;
    }
    if (message == NULL) {
        printf("Message NULL pointer\n");
        return;
    }
    if (ringBuffer->bufferSize >8) {
        printf("Buffer full\n");
        return;
    }

    // Create New Node
    BufferNode *newNode;
    newNode = (BufferNode *)malloc(sizeof(BufferNode));
    if (newNode == NULL) {
        printf("Malloc Failed");
        return;
    }

    // Copy message to new node
    newNode->message = copyMessage(message);
    if (newNode->message == NULL) {
        free(newNode);
        printf("Message NULL pointer\n");
        return;
    }

    // First time RBInit
    if (!ringBuffer->read || !ringBuffer->write) {
        newNode->next = newNode; // Makes ringBuffer a ring
        ringBuffer->read = newNode;
        ringBuffer->write = newNode;
        ringBuffer->bufferSize = 1;
        return;
    }

    // Every other time
    newNode->next = ringBuffer->read;
    ringBuffer->write->next = newNode;
    ringBuffer->write = newNode;
    ringBuffer->bufferSize += 1;
}

char *bufferRead(RingBuffer *ringBuffer) {
    // Error checks
    if (ringBuffer == NULL) {
        printf("RingBuffer NULL pointer\n");
        return NULL;
    }
    if (ringBuffer->bufferSize <= 0) {
        printf("Buffer Empty\n");
        return NULL;
    }
    if (ringBuffer->read == NULL || ringBuffer->write == NULL) {
        ringBuffer->bufferSize = 0;
        printf("Buffer Empty\n");
        return NULL;
    }

    // Get oldest node and copy message
    BufferNode *oldestNode = ringBuffer->read;
    char *message; // temp message to return
    if (oldestNode->message != NULL) {
        message = copyMessage(oldestNode->message);
    } else {
        printf("Buffer Message Empty");
        message = NULL;
    }

    // Clean up

    // If there was only one node
    if (ringBuffer->read == ringBuffer->write) {
        ringBuffer->read = NULL;
        ringBuffer->write = NULL;
        ringBuffer->bufferSize = 0;
        freeBufferNode(oldestNode);
        return message;
    }

    // All other times
    ringBuffer->read = ringBuffer->read->next;
    ringBuffer->bufferSize -= 1;
    freeBufferNode(oldestNode); // destroy the node
    return message;
}

void freeBufferNode(BufferNode *node) {
    if (node == NULL) {
        return;
    }

    free(node->message);
    free(node);
}
