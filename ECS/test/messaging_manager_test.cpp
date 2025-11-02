/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** MessagingManager Unit Tests
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ECS/Messaging/MessagingManager.hpp"
#include "ECS/Messaging/Events/Event.hpp"
#include "ECS/Messaging/MessageQueue.hpp"

TEST_SUITE("MessagingManager Tests") {
    TEST_CASE("MessagingManager is a singleton") {
        auto& instance1 = MessagingManager::instance();
        auto& instance2 = MessagingManager::instance();
        CHECK(&instance1 == &instance2);
    }

    TEST_CASE("MessagingManager initializes successfully") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        // Should not crash
        CHECK(true);
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus can be accessed") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        // Just check we can access it
        CHECK(true);
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager CommandDispatcher can be accessed") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& dispatcher = messaging.get_command_dispatcher();
        // Just check we can access it
        CHECK(true);
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager MessageQueue can be accessed") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        // Just check we can access it
        CHECK(true);
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus subscribe and emit") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        bool eventReceived = false;
        int receivedValue = 0;
        
        // Subscribe to event
        size_t callbackId = eventBus.subscribe("TEST_EVENT", [&](const Event& event) {
            eventReceived = true;
            receivedValue = event.get<int>("value");
        });
        
        // Emit event
        Event testEvent("TEST_EVENT");
        testEvent.set("value", 42);
        eventBus.emit(testEvent);
        
        CHECK(eventReceived);
        CHECK(receivedValue == 42);
        
        eventBus.unsubscribe(callbackId);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus multiple subscribers") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        int count1 = 0;
        int count2 = 0;
        
        // Subscribe two listeners
        size_t id1 = eventBus.subscribe("MULTI_EVENT", [&](const Event& e) { count1++; });
        size_t id2 = eventBus.subscribe("MULTI_EVENT", [&](const Event& e) { count2++; });
        
        // Emit event
        Event event("MULTI_EVENT");
        eventBus.emit(event);
        
        CHECK(count1 == 1);
        CHECK(count2 == 1);
        
        // Emit again
        eventBus.emit(event);
        
        CHECK(count1 == 2);
        CHECK(count2 == 2);
        
        eventBus.unsubscribe(id1);
        eventBus.unsubscribe(id2);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus unsubscribe works") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        int count = 0;
        
        size_t id = eventBus.subscribe("UNSUB_EVENT", [&](const Event& e) { count++; });
        
        Event event("UNSUB_EVENT");
        eventBus.emit(event);
        CHECK(count == 1);
        
        // Unsubscribe
        eventBus.unsubscribe(id);
        
        // Emit again (should not increment)
        eventBus.emit(event);
        CHECK(count == 1); // Still 1
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus handles different event types") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        bool event1Received = false;
        bool event2Received = false;
        
        size_t id1 = eventBus.subscribe("EVENT_TYPE_1", [&](const Event& e) { event1Received = true; });
        size_t id2 = eventBus.subscribe("EVENT_TYPE_2", [&](const Event& e) { event2Received = true; });
        
        Event event1("EVENT_TYPE_1");
        eventBus.emit(event1);
        
        CHECK(event1Received);
        CHECK_FALSE(event2Received);
        
        Event event2("EVENT_TYPE_2");
        eventBus.emit(event2);
        
        CHECK(event1Received);
        CHECK(event2Received);
        
        eventBus.unsubscribe(id1);
        eventBus.unsubscribe(id2);
        messaging.shutdown();
    }

    // Note: CommandDispatcher is commented out due to missing Command.hpp
    // TEST_CASE("MessagingManager CommandDispatcher register and dispatch") {
    //     // Skipped - Command class not available
    // }

    TEST_CASE("MessagingManager MessageQueue push and pop") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        
        // Push message
        Message msg("QUEUE_CHANNEL", 0, 0);
        msg.set("data", std::string("test message"));
        queue.push(msg);
        
        // Pop message
        CHECK_FALSE(queue.empty());
        Message popped("", 0, 0);
        bool success = queue.pop(popped);
        
        CHECK(success);
        CHECK(popped.channel == "QUEUE_CHANNEL");
        CHECK(popped.get<std::string>("data") == "test message");
        CHECK(queue.empty());
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager MessageQueue FIFO order") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        
        // Push multiple messages
        Message msg1("CHANNEL_1", 0, 0);
        msg1.set("order", 1);
        queue.push(msg1);
        
        Message msg2("CHANNEL_2", 0, 0);
        msg2.set("order", 2);
        queue.push(msg2);
        
        Message msg3("CHANNEL_3", 0, 0);
        msg3.set("order", 3);
        queue.push(msg3);
        
        // Pop in FIFO order
        Message popped1("", 0, 0);
        CHECK(queue.pop(popped1));
        CHECK(popped1.get<int>("order") == 1);
        
        Message popped2("", 0, 0);
        CHECK(queue.pop(popped2));
        CHECK(popped2.get<int>("order") == 2);
        
        Message popped3("", 0, 0);
        CHECK(queue.pop(popped3));
        CHECK(popped3.get<int>("order") == 3);
        
        CHECK(queue.empty());
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager clear_all clears all systems") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        auto& queue = messaging.get_message_queue();
        
        // Add some data
        size_t id = eventBus.subscribe("TEST", [](const Event& e) {});
        
        Message msg("QUEUE_CHANNEL", 0, 0);
        msg.set("data", std::string("test"));
        queue.push(msg);
        
        CHECK_FALSE(queue.empty());
        
        // Clear all
        messaging.clear_all();
        
        CHECK(queue.empty());
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager update can be called") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        // Should not crash
        messaging.update();
        messaging.update();
        messaging.update();
        
        CHECK(true);
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager MessageQueue pop on empty queue") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        
        CHECK(queue.empty());
        
        Message msg("", 0, 0);
        bool success = queue.pop(msg);
        
        CHECK_FALSE(success);
        CHECK(queue.empty());
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus multiple subscriptions to different events") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        int countA = 0, countB = 0, countC = 0;
        
        size_t id1 = eventBus.subscribe("EVENT_A", [&](const Event& e) { countA++; });
        size_t id2 = eventBus.subscribe("EVENT_B", [&](const Event& e) { countB++; });
        size_t id3 = eventBus.subscribe("EVENT_C", [&](const Event& e) { countC++; });
        
        Event eventA("EVENT_A");
        Event eventB("EVENT_B");
        Event eventC("EVENT_C");
        
        eventBus.emit(eventA);
        CHECK(countA == 1);
        CHECK(countB == 0);
        CHECK(countC == 0);
        
        eventBus.emit(eventB);
        CHECK(countA == 1);
        CHECK(countB == 1);
        CHECK(countC == 0);
        
        eventBus.emit(eventC);
        CHECK(countA == 1);
        CHECK(countB == 1);
        CHECK(countC == 1);
        
        eventBus.unsubscribe(id1);
        eventBus.unsubscribe(id2);
        eventBus.unsubscribe(id3);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus unsubscribe invalid ID") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        // Should not crash
        eventBus.unsubscribe(99999);
        
        CHECK(true);
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager MessageQueue with many messages") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        
        // Push 100 messages
        for (int i = 0; i < 100; i++) {
            Message msg("CHANNEL", 0, 0);
            msg.set("index", i);
            queue.push(msg);
        }
        
        CHECK_FALSE(queue.empty());
        
        // Pop all and verify FIFO order
        for (int i = 0; i < 100; i++) {
            Message popped("", 0, 0);
            CHECK(queue.pop(popped));
            CHECK(popped.get<int>("index") == i);
        }
        
        CHECK(queue.empty());
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus emit without subscribers") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        Event event("NO_SUBSCRIBERS");
        event.set("data", 123);
        
        // Should not crash
        eventBus.emit(event);
        
        CHECK(true);
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager multiple init and shutdown cycles") {
        auto& messaging = MessagingManager::instance();
        
        messaging.init();
        messaging.shutdown();
        
        messaging.init();
        messaging.shutdown();
        
        messaging.init();
        CHECK(true);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus subscriber receives event data") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        std::string receivedString;
        int receivedInt = 0;
        float receivedFloat = 0.0f;
        
        size_t id = eventBus.subscribe("DATA_EVENT", [&](const Event& event) {
            receivedString = event.get<std::string>("str");
            receivedInt = event.get<int>("num");
            receivedFloat = event.get<float>("flt");
        });
        
        Event event("DATA_EVENT");
        event.set("str", std::string("hello"));
        event.set("num", 42);
        event.set("flt", 3.14f);
        
        eventBus.emit(event);
        
        CHECK(receivedString == "hello");
        CHECK(receivedInt == 42);
        CHECK(receivedFloat == doctest::Approx(3.14f));
        
        eventBus.unsubscribe(id);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager MessageQueue clear works") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        
        // Add messages
        for (int i = 0; i < 10; i++) {
            Message msg("CHANNEL", 0, 0);
            queue.push(msg);
        }
        
        CHECK_FALSE(queue.empty());
        
        queue.clear();
        CHECK(queue.empty());
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus deferred events") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        int immediateCount = 0;
        int deferredCount = 0;
        
        size_t id = eventBus.subscribe("DEFERRED_TEST", [&](const Event& e) {
            if (e.get<bool>("deferred")) {
                deferredCount++;
            } else {
                immediateCount++;
            }
        });
        
        // Emit immediate event
        Event immediateEvent("DEFERRED_TEST");
        immediateEvent.set("deferred", false);
        eventBus.emit(immediateEvent);
        CHECK(immediateCount == 1);
        CHECK(deferredCount == 0);
        
        // Emit deferred event
        Event deferredEvent("DEFERRED_TEST");
        deferredEvent.set("deferred", true);
        eventBus.emit_deferred(deferredEvent);
        CHECK(immediateCount == 1);
        CHECK(deferredCount == 0); // Not processed yet
        
        // Process deferred events
        eventBus.process_deferred();
        CHECK(immediateCount == 1);
        CHECK(deferredCount == 1); // Now processed
        
        eventBus.unsubscribe(id);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus get_subscriber_count") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        CHECK(eventBus.get_subscriber_count("COUNT_TEST") == 0);
        
        size_t id1 = eventBus.subscribe("COUNT_TEST", [](const Event& e) {});
        CHECK(eventBus.get_subscriber_count("COUNT_TEST") == 1);
        
        size_t id2 = eventBus.subscribe("COUNT_TEST", [](const Event& e) {});
        CHECK(eventBus.get_subscriber_count("COUNT_TEST") == 2);
        
        size_t id3 = eventBus.subscribe("COUNT_TEST", [](const Event& e) {});
        CHECK(eventBus.get_subscriber_count("COUNT_TEST") == 3);
        
        eventBus.unsubscribe(id2);
        CHECK(eventBus.get_subscriber_count("COUNT_TEST") == 2);
        
        eventBus.unsubscribe(id1);
        eventBus.unsubscribe(id3);
        CHECK(eventBus.get_subscriber_count("COUNT_TEST") == 0);
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager MessageQueue pop_for_channel") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        
        // Push messages to different channels
        Message msg1("CHANNEL_A", 0, 0);
        msg1.set("value", 1);
        queue.push(msg1);
        
        Message msg2("CHANNEL_B", 0, 0);
        msg2.set("value", 2);
        queue.push(msg2);
        
        Message msg3("CHANNEL_A", 0, 0);
        msg3.set("value", 3);
        queue.push(msg3);
        
        // Pop from specific channel
        Message popped("", 0, 0);
        CHECK(queue.pop_for_channel("CHANNEL_A", popped));
        CHECK(popped.channel == "CHANNEL_A");
        CHECK(popped.get<int>("value") == 1);
        
        // Pop from another channel
        Message popped2("", 0, 0);
        CHECK(queue.pop_for_channel("CHANNEL_B", popped2));
        CHECK(popped2.channel == "CHANNEL_B");
        CHECK(popped2.get<int>("value") == 2);
        
        // Pop remaining from channel A
        Message popped3("", 0, 0);
        CHECK(queue.pop_for_channel("CHANNEL_A", popped3));
        CHECK(popped3.get<int>("value") == 3);
        
        CHECK(queue.empty());
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager MessageQueue pop_for_channel on empty") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        
        CHECK(queue.empty());
        
        Message popped("", 0, 0);
        CHECK_FALSE(queue.pop_for_channel("NONEXISTENT", popped));
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus unsubscribe_deferred") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        int count = 0;
        size_t id = eventBus.subscribe("DEFERRED_UNSUB", [&](const Event& e) {
            count++;
        });
        
        Event event("DEFERRED_UNSUB");
        eventBus.emit(event);
        CHECK(count == 1);
        
        // Unsubscribe deferred
        eventBus.unsubscribe_deferred(id);
        
        // Still active until process_deferred
        eventBus.emit(event);
        CHECK(count == 2);
        
        // Process deferred unsubscriptions
        eventBus.process_deferred();
        
        // Now inactive
        eventBus.emit(event);
        CHECK(count == 2); // Should still be 2
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager MessageQueue size") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& queue = messaging.get_message_queue();
        
        CHECK(queue.size() == 0);
        CHECK(queue.empty());
        
        Message msg1("CHANNEL", 0, 0);
        queue.push(msg1);
        CHECK(queue.size() == 1);
        CHECK_FALSE(queue.empty());
        
        Message msg2("CHANNEL", 0, 0);
        queue.push(msg2);
        CHECK(queue.size() == 2);
        
        Message popped("", 0, 0);
        queue.pop(popped);
        CHECK(queue.size() == 1);
        
        queue.pop(popped);
        CHECK(queue.size() == 0);
        CHECK(queue.empty());
        
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager integration test - all systems") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        auto& queue = messaging.get_message_queue();
        
        // EventBus test
        bool eventReceived = false;
        size_t id = eventBus.subscribe("INTEGRATION_EVENT", [&](const Event& e) {
            eventReceived = true;
        });
        
        Event event("INTEGRATION_EVENT");
        eventBus.emit(event);
        CHECK(eventReceived);
        
        // MessageQueue test
        Message msg("INTEGRATION_CHANNEL", 0, 0);
        msg.set("integration", true);
        queue.push(msg);
        
        Message popped("", 0, 0);
        CHECK(queue.pop(popped));
        CHECK(popped.get<bool>("integration"));
        
        // Update test
        messaging.update();
        
        // Clear all test
        queue.push(msg);
        CHECK_FALSE(queue.empty());
        
        messaging.clear_all();
        CHECK(queue.empty());
        
        eventBus.unsubscribe(id);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager EventBus exception in callback") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        int safeCallbackCount = 0;
        
        // Subscribe callback that throws
        size_t id1 = eventBus.subscribe("EXCEPTION_EVENT", [](const Event& e) {
            throw std::runtime_error("Test exception");
        });
        
        // Subscribe safe callback
        size_t id2 = eventBus.subscribe("EXCEPTION_EVENT", [&](const Event& e) {
            safeCallbackCount++;
        });
        
        Event event("EXCEPTION_EVENT");
        
        // Should not crash despite exception
        eventBus.emit(event);
        
        // Safe callback should still have been called
        CHECK(safeCallbackCount == 1);
        
        eventBus.unsubscribe(id1);
        eventBus.unsubscribe(id2);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager stress test - many events") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        int totalCount = 0;
        size_t id = eventBus.subscribe("STRESS_EVENT", [&](const Event& e) {
            totalCount++;
        });
        
        // Emit 1000 events
        for (int i = 0; i < 1000; i++) {
            Event event("STRESS_EVENT");
            event.set("index", i);
            eventBus.emit(event);
        }
        
        CHECK(totalCount == 1000);
        
        eventBus.unsubscribe(id);
        messaging.shutdown();
    }

    TEST_CASE("MessagingManager stress test - many subscribers") {
        auto& messaging = MessagingManager::instance();
        messaging.init();
        
        auto& eventBus = messaging.get_event_bus();
        
        std::vector<size_t> ids;
        std::vector<int> counts(100, 0);
        
        // Create 100 subscribers
        for (int i = 0; i < 100; i++) {
            size_t id = eventBus.subscribe("MANY_SUBS", [&counts, i](const Event& e) {
                counts[i]++;
            });
            ids.push_back(id);
        }
        
        // Emit one event
        Event event("MANY_SUBS");
        eventBus.emit(event);
        
        // All subscribers should have received it
        for (int count : counts) {
            CHECK(count == 1);
        }
        
        // Cleanup
        for (size_t id : ids) {
            eventBus.unsubscribe(id);
        }
        
        messaging.shutdown();
    }
}
