#pragma once

#include <enet/enet.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace BVA {

enum class NetworkMode {
    None,
    Server,
    Client
};

enum class PacketType : uint8_t {
    PlayerJoin,
    PlayerLeave,
    PlayerMove,
    PlayerAttack,
    PlayerAbility,
    PlayerDamage,
    PlayerHealth,
    GameState,
    ChatMessage,
    Ping
};

struct NetworkPacket {
    PacketType type;
    uint32_t timestamp;
    std::vector<uint8_t> data;
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    bool initialize();
    void shutdown();
    void update(float dt);

    // Server
    bool startServer(uint16_t port, int maxClients = 8);
    void stopServer();
    bool isServer() const { return mode == NetworkMode::Server; }

    // Client
    bool connect(const std::string& hostname, uint16_t port);
    void disconnect();
    bool isConnected() const;
    bool isClient() const { return mode == NetworkMode::Client; }

    // Packet sending
    void sendPacket(const NetworkPacket& packet, bool reliable = true);
    void sendPacketToClient(ENetPeer* peer, const NetworkPacket& packet, bool reliable = true);
    void broadcastPacket(const NetworkPacket& packet, bool reliable = true);

    // Packet callbacks
    using PacketHandler = std::function<void(const NetworkPacket&, ENetPeer*)>;
    void registerPacketHandler(PacketType type, PacketHandler handler);

    // Stats
    uint32_t getPing() const { return ping; }
    uint32_t getPacketsSent() const { return packetsSent; }
    uint32_t getPacketsReceived() const { return packetsReceived; }

private:
    void processEvents();
    void handlePacket(ENetPacket* packet, ENetPeer* peer);

    NetworkMode mode = NetworkMode::None;
    ENetHost* host = nullptr;
    ENetPeer* serverPeer = nullptr;
    std::vector<ENetPeer*> clients;

    std::unordered_map<PacketType, PacketHandler> packetHandlers;

    uint32_t ping = 0;
    uint32_t packetsSent = 0;
    uint32_t packetsReceived = 0;
};

} // namespace BVA
