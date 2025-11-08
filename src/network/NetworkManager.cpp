#include "network/NetworkManager.hpp"
#include <iostream>

namespace BVA {

NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager() {
    shutdown();
}

bool NetworkManager::initialize() {
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet!" << std::endl;
        return false;
    }

    std::cout << "Network manager initialized with ENet" << std::endl;
    return true;
}

void NetworkManager::shutdown() {
    stopServer();
    disconnect();

    if (host) {
        enet_host_destroy(host);
        host = nullptr;
    }

    enet_deinitialize();
}

void NetworkManager::update(float dt) {
    if (!host) return;

    processEvents();
}

bool NetworkManager::startServer(uint16_t port, int maxClients) {
    if (mode != NetworkMode::None) {
        std::cerr << "Already in network mode!" << std::endl;
        return false;
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    host = enet_host_create(&address, maxClients, 2, 0, 0);
    if (!host) {
        std::cerr << "Failed to create ENet server!" << std::endl;
        return false;
    }

    mode = NetworkMode::Server;
    std::cout << "Server started on port " << port << std::endl;
    return true;
}

void NetworkManager::stopServer() {
    if (mode != NetworkMode::Server) return;

    // Disconnect all clients
    for (auto* peer : clients) {
        enet_peer_disconnect(peer, 0);
    }

    // Process disconnect events
    ENetEvent event;
    while (enet_host_service(host, &event, 3000) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            enet_packet_destroy(event.packet);
        }
    }

    clients.clear();
    mode = NetworkMode::None;
    std::cout << "Server stopped" << std::endl;
}

bool NetworkManager::connect(const std::string& hostname, uint16_t port) {
    if (mode != NetworkMode::None) {
        std::cerr << "Already in network mode!" << std::endl;
        return false;
    }

    host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!host) {
        std::cerr << "Failed to create ENet client!" << std::endl;
        return false;
    }

    ENetAddress address;
    enet_address_set_host(&address, hostname.c_str());
    address.port = port;

    serverPeer = enet_host_connect(host, &address, 2, 0);
    if (!serverPeer) {
        std::cerr << "Failed to connect to server!" << std::endl;
        enet_host_destroy(host);
        host = nullptr;
        return false;
    }

    // Wait for connection
    ENetEvent event;
    if (enet_host_service(host, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        mode = NetworkMode::Client;
        std::cout << "Connected to server at " << hostname << ":" << port << std::endl;
        return true;
    }

    enet_peer_reset(serverPeer);
    enet_host_destroy(host);
    host = nullptr;
    serverPeer = nullptr;
    std::cerr << "Connection to server failed!" << std::endl;
    return false;
}

void NetworkManager::disconnect() {
    if (mode != NetworkMode::Client || !serverPeer) return;

    enet_peer_disconnect(serverPeer, 0);

    // Wait for disconnect
    ENetEvent event;
    while (enet_host_service(host, &event, 3000) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            break;
        }
    }

    serverPeer = nullptr;
    mode = NetworkMode::None;
    std::cout << "Disconnected from server" << std::endl;
}

bool NetworkManager::isConnected() const {
    return mode == NetworkMode::Client && serverPeer != nullptr;
}

void NetworkManager::sendPacket(const NetworkPacket& packet, bool reliable) {
    if (mode == NetworkMode::Client && serverPeer) {
        sendPacketToClient(serverPeer, packet, reliable);
    }
}

void NetworkManager::sendPacketToClient(ENetPeer* peer, const NetworkPacket& packet, bool reliable) {
    if (!peer) return;

    // Serialize packet (simplified)
    std::vector<uint8_t> buffer;
    buffer.push_back(static_cast<uint8_t>(packet.type));
    buffer.insert(buffer.end(), packet.data.begin(), packet.data.end());

    ENetPacket* enetPacket = enet_packet_create(
        buffer.data(),
        buffer.size(),
        reliable ? ENET_PACKET_FLAG_RELIABLE : 0
    );

    enet_peer_send(peer, 0, enetPacket);
    packetsSent++;
}

void NetworkManager::broadcastPacket(const NetworkPacket& packet, bool reliable) {
    if (mode != NetworkMode::Server) return;

    for (auto* peer : clients) {
        sendPacketToClient(peer, packet, reliable);
    }
}

void NetworkManager::registerPacketHandler(PacketType type, PacketHandler handler) {
    packetHandlers[type] = handler;
}

void NetworkManager::processEvents() {
    if (!host) return;

    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                if (mode == NetworkMode::Server) {
                    clients.push_back(event.peer);
                    std::cout << "Client connected from "
                              << (event.peer->address.host & 0xFF) << "."
                              << ((event.peer->address.host >> 8) & 0xFF) << "."
                              << ((event.peer->address.host >> 16) & 0xFF) << "."
                              << ((event.peer->address.host >> 24) & 0xFF)
                              << ":" << event.peer->address.port << std::endl;
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                handlePacket(event.packet, event.peer);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                if (mode == NetworkMode::Server) {
                    auto it = std::find(clients.begin(), clients.end(), event.peer);
                    if (it != clients.end()) {
                        clients.erase(it);
                    }
                    std::cout << "Client disconnected" << std::endl;
                }
                break;

            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}

void NetworkManager::handlePacket(ENetPacket* packet, ENetPeer* peer) {
    if (!packet || packet->dataLength < 1) return;

    PacketType type = static_cast<PacketType>(packet->data[0]);

    // Create network packet
    NetworkPacket netPacket;
    netPacket.type = type;
    netPacket.data.assign(packet->data + 1, packet->data + packet->dataLength);

    // Call handler
    auto it = packetHandlers.find(type);
    if (it != packetHandlers.end()) {
        it->second(netPacket, peer);
    }

    packetsReceived++;
}

} // namespace BVA
