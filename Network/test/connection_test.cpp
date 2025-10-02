#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "connection.hpp"
#include <asio.hpp>
#include <string>
#include <vector>

using RType::Network::Connection;

TEST_CASE("Connection: send() envoie bien un datagramme et reste active")
{
    asio::io_context io;

    // Socket receveur bindée sur loopback, port auto (0)
    asio::ip::udp::socket receiver(io, {asio::ip::address_v4::loopback(), 0});
    const auto recv_ep = receiver.local_endpoint();

    // Socket émettrice
    auto sender = std::make_shared<asio::ip::udp::socket>(
        io, asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0));

    // Connection à tester (envoie vers le receveur)
    Connection conn(recv_ep, sender);

    // Préparer une réception asynchrone côté receveur
    std::array<char, 1024> buffer{};
    std::size_t received = 0;
    bool got = false;
    asio::ip::udp::endpoint from;

    receiver.async_receive_from(
        asio::buffer(buffer),
        from,
        [&](const std::error_code& ec, std::size_t n) {
            CHECK_MESSAGE(!ec, ("Receive error: " + ec.message()).c_str());
            received = n;
            got = true;
        }
    );

    // Envoyer un message
    const char msg[] = "hello-udp";
    conn.send(msg, sizeof(msg)-1);

    // Lancer l'event loop jusqu'à ce qu'il n'y ait plus de travail
    io.run();

    CHECK(got == true);
    CHECK(received == sizeof(msg)-1);
    CHECK(std::string(buffer.data(), received) == "hello-udp");
    CHECK(conn.is_active() == true);

    // L'activité doit être mise à jour par le handler d'envoi
    auto idle = conn.get_idle_time();
    CHECK(idle.count() >= 0);
}

TEST_CASE("Connection: disconnect() rend la connexion inactive et send() devient no-op")
{
    asio::io_context io;
    asio::ip::udp::socket receiver(io, {asio::ip::address_v4::loopback(), 0});
    auto sender = std::make_shared<asio::ip::udp::socket>(
        io, asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0));

    Connection conn(receiver.local_endpoint(), sender);

    conn.disconnect();
    CHECK(conn.is_active() == false);

    // Tentative d'envoi : ne doit pas crasher ni relancer l'io_context
    conn.send("x", 1);
    // io.run() ne doit exécuter aucun handler (0)
    io.run();
    CHECK(conn.is_active() == false);
}

TEST_CASE("Connection: erreur d'envoi (socket fermée) -> active_ passe à false")
{
    asio::io_context io;
    asio::ip::udp::socket receiver(io, {asio::ip::address_v4::loopback(), 0});
    auto sender = std::make_shared<asio::ip::udp::socket>(
        io, asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0));

    Connection conn(receiver.local_endpoint(), sender);

    // Fermer la socket avant l'envoi pour provoquer une erreur immédiate
    sender->close();

    conn.send("data", 4);
    io.run(); // traite le handler d'erreur de async_send_to

    CHECK(conn.is_active() == false);
}
