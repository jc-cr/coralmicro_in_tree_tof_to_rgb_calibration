

    // Initialize USB communication
    std::string usb_ip;
    if (!GetUsbIpAddress(&usb_ip)) {
        printf("Failed to get USB IP Address\r\n");
        vTaskSuspend(nullptr);
    }
    printf("Starting Stream Service on: %s\r\n", usb_ip.c_str());


    // Setup RPC endpoint
    jsonrpc_init(nullptr, nullptr);
    jsonrpc_export("get_synchronized_frame", GetSynchronizedFrame);
    UseHttpServer(new JsonRpcHttpServer);

    vTaskSuspend(nullptr);