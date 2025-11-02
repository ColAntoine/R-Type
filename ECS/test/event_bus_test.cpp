#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Must be before raylib.h to avoid Windows GDI conflicts
#define NOGDI
#define NOMINMAX

#include "ECS/Messaging/EventBus.hpp"
#include <stdexcept>

TEST_SUITE("EventBus Tests") {
    TEST_CASE("EventBus subscribe returns unique IDs") {
        EventBus bus;
        
        auto id1 = bus.subscribe("EVENT_A", [](const Event& e) {});
        auto id2 = bus.subscribe("EVENT_A", [](const Event& e) {});
        auto id3 = bus.subscribe("EVENT_B", [](const Event& e) {});
        
        CHECK(id1 != id2);
        CHECK(id2 != id3);
        CHECK(id1 != id3);
    }

    TEST_CASE("EventBus emit triggers subscribed callbacks") {
        EventBus bus;
        
        int count = 0;
        bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        Event event("TEST");
        bus.emit(event);
        
        CHECK(count == 1);
    }

    TEST_CASE("EventBus emit triggers multiple subscribers") {
        EventBus bus;
        
        int count1 = 0, count2 = 0, count3 = 0;
        bus.subscribe("TEST", [&](const Event& e) { count1++; });
        bus.subscribe("TEST", [&](const Event& e) { count2++; });
        bus.subscribe("TEST", [&](const Event& e) { count3++; });
        
        Event event("TEST");
        bus.emit(event);
        
        CHECK(count1 == 1);
        CHECK(count2 == 1);
        CHECK(count3 == 1);
    }

    TEST_CASE("EventBus emit does not trigger wrong event type") {
        EventBus bus;
        
        int countA = 0, countB = 0;
        bus.subscribe("EVENT_A", [&](const Event& e) { countA++; });
        bus.subscribe("EVENT_B", [&](const Event& e) { countB++; });
        
        Event eventA("EVENT_A");
        bus.emit(eventA);
        
        CHECK(countA == 1);
        CHECK(countB == 0);
    }

    TEST_CASE("EventBus unsubscribe removes callback") {
        EventBus bus;
        
        int count = 0;
        auto id = bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        Event event("TEST");
        bus.emit(event);
        CHECK(count == 1);
        
        bus.unsubscribe(id);
        bus.emit(event);
        CHECK(count == 1); // Still 1, not incremented
    }

    TEST_CASE("EventBus unsubscribe with invalid ID does not crash") {
        EventBus bus;
        
        // Should not crash
        bus.unsubscribe(999);
        bus.unsubscribe(123456);
        
        CHECK(true);
    }

    TEST_CASE("EventBus unsubscribe removes only specified callback") {
        EventBus bus;
        
        int count1 = 0, count2 = 0;
        auto id1 = bus.subscribe("TEST", [&](const Event& e) { count1++; });
        auto id2 = bus.subscribe("TEST", [&](const Event& e) { count2++; });
        
        bus.unsubscribe(id1);
        
        Event event("TEST");
        bus.emit(event);
        
        CHECK(count1 == 0); // Not called
        CHECK(count2 == 1); // Still called
    }

    TEST_CASE("EventBus emit_deferred queues event") {
        EventBus bus;
        
        int count = 0;
        bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        Event event("TEST");
        bus.emit_deferred(event);
        
        // Not processed yet
        CHECK(count == 0);
        CHECK(bus.get_deferred_count() == 1);
    }

    TEST_CASE("EventBus process_deferred processes queued events") {
        EventBus bus;
        
        int count = 0;
        bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        Event event1("TEST");
        Event event2("TEST");
        bus.emit_deferred(event1);
        bus.emit_deferred(event2);
        
        CHECK(count == 0);
        
        bus.process_deferred();
        
        CHECK(count == 2);
        CHECK(bus.get_deferred_count() == 0);
    }

    TEST_CASE("EventBus process_deferred processes in FIFO order") {
        EventBus bus;
        
        std::vector<int> order;
        bus.subscribe("TEST", [&](const Event& e) {
            order.push_back(e.get<int>("value"));
        });
        
        Event e1("TEST"); e1.set("value", 1);
        Event e2("TEST"); e2.set("value", 2);
        Event e3("TEST"); e3.set("value", 3);
        
        bus.emit_deferred(e1);
        bus.emit_deferred(e2);
        bus.emit_deferred(e3);
        
        bus.process_deferred();
        
        REQUIRE(order.size() == 3);
        CHECK(order[0] == 1);
        CHECK(order[1] == 2);
        CHECK(order[2] == 3);
    }

    TEST_CASE("EventBus unsubscribe_deferred delays unsubscription") {
        EventBus bus;
        
        int count = 0;
        auto id = bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        Event event("TEST");
        bus.emit(event);
        CHECK(count == 1);
        
        // Deferred unsubscribe
        bus.unsubscribe_deferred(id);
        
        // Still works
        bus.emit(event);
        CHECK(count == 2);
        
        // Process deferred operations
        bus.process_deferred();
        
        // Now unsubscribed
        bus.emit(event);
        CHECK(count == 2); // Not incremented
    }

    TEST_CASE("EventBus handles callback exceptions gracefully") {
        EventBus bus;
        
        int count1 = 0, count2 = 0;
        
        // First callback throws
        bus.subscribe("TEST", [&](const Event& e) {
            count1++;
            throw std::runtime_error("Test exception");
        });
        
        // Second callback should still execute
        bus.subscribe("TEST", [&](const Event& e) {
            count2++;
        });
        
        Event event("TEST");
        bus.emit(event);
        
        // Both should have been called
        CHECK(count1 == 1);
        CHECK(count2 == 1);
    }

    TEST_CASE("EventBus clear_all removes all subscribers") {
        EventBus bus;
        
        int count = 0;
        bus.subscribe("TEST_A", [&](const Event& e) { count++; });
        bus.subscribe("TEST_B", [&](const Event& e) { count++; });
        bus.subscribe("TEST_C", [&](const Event& e) { count++; });
        
        bus.clear_all();
        
        Event eventA("TEST_A");
        Event eventB("TEST_B");
        Event eventC("TEST_C");
        
        bus.emit(eventA);
        bus.emit(eventB);
        bus.emit(eventC);
        
        CHECK(count == 0); // No callbacks executed
    }

    TEST_CASE("EventBus clear_all clears deferred events") {
        EventBus bus;
        
        int count = 0;
        bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        Event event("TEST");
        bus.emit_deferred(event);
        bus.emit_deferred(event);
        
        CHECK(bus.get_deferred_count() == 2);
        
        bus.clear_all();
        
        CHECK(bus.get_deferred_count() == 0);
        
        bus.process_deferred();
        CHECK(count == 0); // Nothing processed
    }

    TEST_CASE("EventBus clear_all clears pending unsubscribes") {
        EventBus bus;
        
        int count = 0;
        auto id = bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        bus.unsubscribe_deferred(id);
        bus.clear_all();
        
        // Callback was cleared by clear_all, so unsubscribe is moot
        bus.process_deferred();
        
        Event event("TEST");
        bus.emit(event);
        CHECK(count == 0); // Cleared
    }

    TEST_CASE("EventBus get_subscriber_count returns correct count") {
        EventBus bus;
        
        CHECK(bus.get_subscriber_count("TEST") == 0);
        
        auto id1 = bus.subscribe("TEST", [](const Event& e) {});
        CHECK(bus.get_subscriber_count("TEST") == 1);
        
        auto id2 = bus.subscribe("TEST", [](const Event& e) {});
        CHECK(bus.get_subscriber_count("TEST") == 2);
        
        auto id3 = bus.subscribe("OTHER", [](const Event& e) {});
        CHECK(bus.get_subscriber_count("TEST") == 2);
        CHECK(bus.get_subscriber_count("OTHER") == 1);
        
        bus.unsubscribe(id1);
        CHECK(bus.get_subscriber_count("TEST") == 1);
    }

    TEST_CASE("EventBus get_subscriber_count for non-existent type") {
        EventBus bus;
        
        CHECK(bus.get_subscriber_count("NON_EXISTENT") == 0);
    }

    TEST_CASE("EventBus handles many subscribers") {
        EventBus bus;
        
        int count = 0;
        std::vector<EventBus::CallbackId> ids;
        
        // Subscribe 100 callbacks
        for (int i = 0; i < 100; i++) {
            auto id = bus.subscribe("TEST", [&](const Event& e) { count++; });
            ids.push_back(id);
        }
        
        CHECK(bus.get_subscriber_count("TEST") == 100);
        
        Event event("TEST");
        bus.emit(event);
        
        CHECK(count == 100);
        
        // Unsubscribe half
        for (int i = 0; i < 50; i++) {
            bus.unsubscribe(ids[i]);
        }
        
        CHECK(bus.get_subscriber_count("TEST") == 50);
        
        count = 0;
        bus.emit(event);
        CHECK(count == 50);
    }

    TEST_CASE("EventBus handles many deferred events") {
        EventBus bus;
        
        int count = 0;
        bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        Event event("TEST");
        for (int i = 0; i < 50; i++) {
            bus.emit_deferred(event);
        }
        
        CHECK(bus.get_deferred_count() == 50);
        
        bus.process_deferred();
        
        CHECK(count == 50);
        CHECK(bus.get_deferred_count() == 0);
    }

    TEST_CASE("EventBus emit with no subscribers does not crash") {
        EventBus bus;
        
        Event event("NO_SUBSCRIBERS");
        event.set("data", 123);
        
        // Should not crash
        bus.emit(event);
        
        CHECK(true);
    }

    TEST_CASE("EventBus multiple unsubscribe of same ID") {
        EventBus bus;
        
        int count = 0;
        auto id = bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        bus.unsubscribe(id);
        bus.unsubscribe(id); // Second time should not crash
        bus.unsubscribe(id); // Third time
        
        Event event("TEST");
        bus.emit(event);
        
        CHECK(count == 0);
    }

    TEST_CASE("EventBus callback can access event data") {
        EventBus bus;
        
        std::string received_str;
        int received_int = 0;
        float received_float = 0.0f;
        
        bus.subscribe("DATA", [&](const Event& e) {
            received_str = e.get<std::string>("str");
            received_int = e.get<int>("num");
            received_float = e.get<float>("flt");
        });
        
        Event event("DATA");
        event.set("str", std::string("hello world"));
        event.set("num", 42);
        event.set("flt", 3.14f);
        
        bus.emit(event);
        
        CHECK(received_str == "hello world");
        CHECK(received_int == 42);
        CHECK(received_float == doctest::Approx(3.14f));
    }

    TEST_CASE("EventBus process_deferred with empty queue") {
        EventBus bus;
        
        // Should not crash
        bus.process_deferred();
        bus.process_deferred();
        
        CHECK(true);
    }

    TEST_CASE("EventBus mixed immediate and deferred emissions") {
        EventBus bus;
        
        std::vector<std::string> order;
        bus.subscribe("TEST", [&](const Event& e) {
            order.push_back(e.get<std::string>("source"));
        });
        
        Event e1("TEST"); e1.set("source", std::string("immediate1"));
        Event e2("TEST"); e2.set("source", std::string("deferred1"));
        Event e3("TEST"); e3.set("source", std::string("immediate2"));
        Event e4("TEST"); e4.set("source", std::string("deferred2"));
        
        bus.emit(e1);
        bus.emit_deferred(e2);
        bus.emit(e3);
        bus.emit_deferred(e4);
        
        REQUIRE(order.size() == 2);
        CHECK(order[0] == "immediate1");
        CHECK(order[1] == "immediate2");
        
        bus.process_deferred();
        
        REQUIRE(order.size() == 4);
        CHECK(order[2] == "deferred1");
        CHECK(order[3] == "deferred2");
    }

    TEST_CASE("EventBus unsubscribe during process_deferred") {
        EventBus bus;
        
        int count = 0;
        EventBus::CallbackId id1, id2;
        
        id1 = bus.subscribe("TEST", [&](const Event& e) { count++; });
        id2 = bus.subscribe("TEST", [&](const Event& e) { count++; });
        
        bus.unsubscribe_deferred(id1);
        
        Event event("TEST");
        bus.emit_deferred(event);
        
        // Both should still be called (deferred unsubscribe not processed yet)
        bus.process_deferred();
        CHECK(count == 2);
        
        // Now id1 should be unsubscribed
        count = 0;
        bus.emit(event);
        CHECK(count == 1); // Only id2
    }

    TEST_CASE("EventBus get_deferred_count accuracy") {
        EventBus bus;
        
        CHECK(bus.get_deferred_count() == 0);
        
        Event event("TEST");
        bus.emit_deferred(event);
        CHECK(bus.get_deferred_count() == 1);
        
        bus.emit_deferred(event);
        bus.emit_deferred(event);
        CHECK(bus.get_deferred_count() == 3);
        
        bus.process_deferred();
        CHECK(bus.get_deferred_count() == 0);
    }
}
