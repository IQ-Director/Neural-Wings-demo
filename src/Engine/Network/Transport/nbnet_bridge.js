// src/Engine/Network/Transport/nbnet_bridge.js

mergeInto(LibraryManager.library, {
    // =========================================================================
    // GAME CLIENT (客户端) 函数实现
    // 对应报错: undefined symbol: __js_game_client_...
    // =========================================================================

    __js_game_client_init: function (server_url_ptr, port) {
        var url = UTF8ToString(server_url_ptr);
        console.log("[NBNet Bridge] CLIENT Init. URL:", url, "Port:", port);

        // 全局对象初始化
        window.NBNetBox = window.NBNetBox || {
            socket: null,
            packetQueue: [],
            connected: false
        };

        // TODO: 在这里实现真正的 WebRTC/WebSocket 连接逻辑
        // window.NBNetBox.socket = new WebSocket(url); ...
    },

    __js_game_client_start: function () {
        console.log("[NBNet Bridge] CLIENT Start");
    },

    __js_game_client_close: function () {
        console.log("[NBNet Bridge] CLIENT Close");
        if (window.NBNetBox && window.NBNetBox.socket) {
            window.NBNetBox.socket.close();
        }
    },

    // 发送数据: C -> JS
    __js_game_client_send_packet: function (data_ptr, data_len, channel) {
        // 将 WASM 内存数据拷贝到 JS
        var data = new Uint8Array(HEAPU8.subarray(data_ptr, data_ptr + data_len));

        // console.log("[NBNet Bridge] CLIENT Send packet, len:", data_len);

        // TODO: 发送给服务器
        // if (window.NBNetBox.socket && window.NBNetBox.connected) window.NBNetBox.socket.send(data);
    },

    // 接收数据: JS -> C
    // 注意：nbnet 驱动通常传入一个 buffer 指针和最大长度
    // 返回值：实际写入的字节数，0 表示无数据
    __js_game_client_dequeue_packet: function (out_buffer_ptr, max_len) {
        if (!window.NBNetBox || window.NBNetBox.packetQueue.length === 0) {
            return 0;
        }

        // 取出一个包
        var packet = window.NBNetBox.packetQueue.shift();

        // 检查缓冲区是否足够
        if (packet.length > max_len) {
            console.error("[NBNet Bridge] Packet too large for C buffer:", packet.length);
            return 0; // 丢弃或处理错误
        }

        // 将 JS 数据写入 C 内存 (HEAPU8)
        HEAPU8.set(packet, out_buffer_ptr);

        return packet.length;
    },


    // =========================================================================
    // GAME SERVER (服务端/P2P主机) 函数实现
    // 为了防止以后切换模式又报错，保留这些定义作为存根(Stub)
    // =========================================================================

    __js_game_server_init: function (address_ptr, port) {
        console.log("[NBNet Bridge] SERVER Init (Stub)");
    },

    __js_game_server_start: function () { },
    __js_game_server_stop: function () { },

    __js_game_server_send_packet_to: function (connectionId, data_ptr, data_len) {
        console.log("[NBNet Bridge] SERVER Send to", connectionId);
    },

    __js_game_server_dequeue_packet: function (out_buffer_ptr, max_len) {
        return 0;
    },

    __js_game_server_close_client_peer: function (connectionId) { }
});