#ifndef __LIBUV_H_ADAPTER__
#define __LIBUV_H_ADAPTER__

#include <sys/types.h>
#include "uv.h"
#include "hiredis.h"
#include "async.h"

typedef struct redisLibuvEvents {
    redisAsyncContext *context;
    struct uv_loop_t *loop;
    int reading, writing;
        uv_prepare_t read_handle, write_handle;
} redisLibuvEvents;

uv_buf_t onAlloc(uv_handle_t* client, size_t suggested_size) {
  uv_buf_t buf;
  buf.base = malloc(suggested_size);
  buf.len = suggested_size;
  return buf;
}

void redisLibuvReadEvent(uv_prepare_t* handle, int status) {
    redisLibuvEvents *e = (redisLibuvEvents*)handle->data;
    redisAsyncHandleRead(e->context);
}

void redisLibuvWriteEvent(uv_prepare_t* handle, int status)
{
    redisLibuvEvents *e = (redisLibuvEvents*)handle->data;
    redisAsyncHandleWrite(e->context);
}

void redisLibuvAddRead(void *privdata)
{
    redisLibuvEvents *e = (redisLibuvEvents*)privdata;
    struct uv_loop *loop = e->loop;
    if (!e->reading) {
        e->reading = 1;
                uv_idle_start(&e->read_handle, redisLibuvReadEvent);
    }
}

void redisLibuvDelRead(void *privdata)
{
    redisLibuvEvents *e = (redisLibuvEvents*)privdata;
    struct uv_loop *loop = e->loop;
    ((void)loop);
    if (e->reading) {
        e->reading = 0;
                uv_idle_stop(&e->read_handle);
    }
}

void redisLibuvAddWrite(void *privdata) {
    redisLibuvEvents *e = (redisLibuvEvents*)privdata;
    struct ev_loop *loop = e->loop;
    ((void)loop);
    if (!e->writing) {
        e->writing = 1;
                uv_idle_start(&e->write_handle, redisLibuvWriteEvent);
    }
}

void redisLibuvDelWrite(void *privdata)
{
    redisLibuvEvents *e = (redisLibuvEvents*)privdata;
    struct uv_loop *loop = e->loop;
    ((void)loop);
    if (e->writing) {
        e->writing = 0;
                uv_idle_stop(&e->write_handle);
    }
}

void redisLibuvCleanup(void *privdata) {
    redisLibuvEvents *e = (redisLibuvEvents*)privdata;
    redisLibuvDelRead(privdata);
    redisLibuvDelWrite(privdata);
    free(e);
}

int redisLibuvAttach(uv_loop_t *loop, redisAsyncContext *ac)
{
    redisContext *c = &(ac->c);
    redisLibuvEvents *e;

    /* Nothing should be attached when something is already attached */
    if (ac->data != NULL)
        return REDIS_ERR;

    // Create container for context and r/w events
    e = (redisLibuvEvents*)malloc(sizeof(*e));
    e->context = ac;
        e->loop = loop;

    /* Register functions to start/stop listening for events */
    ac->ev.addRead = redisLibuvAddRead;
    ac->ev.delRead = redisLibuvDelRead;
    ac->ev.addWrite = redisLibuvAddWrite;
    ac->ev.delWrite = redisLibuvDelWrite;
    ac->ev.cleanup = redisLibuvCleanup;
        ac->ev.data = e;

        // Initialize read/write events
        uv_idle_init(e->loop, &e->read_handle);
        uv_idle_init(e->loop, &e->write_handle);

    e->reading = e->writing = 0;
    e->read_handle.data = e;
        e->write_handle.data = e;

    return REDIS_OK;
}

#endif