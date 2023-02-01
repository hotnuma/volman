#if 0

// (device_cleanup)

void tvm_device_handler_finished(TvmContext *context)
{
    g_return_if_fail(context != NULL);

    if (context->error != NULL)
    {
        g_list_free(context->handlers);
    }
    else
    {
        if (context->handlers != NULL)
            _device_next_handler(context);
    }
}

#endif


