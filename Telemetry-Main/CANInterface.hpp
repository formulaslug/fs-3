#ifndef CAN_INTERFACE_HPP
#define CAN_INTERFACE_HPP

#include "mbed.h"

/**
 * @brief Abstract interface for CAN communication
 * This enables dependency injection and mocking for testing
 */
class ICAN {
public:
    virtual ~ICAN() = default;
    
    /**
     * @brief Read a CAN message if available
     * @param msg Reference to store the received message
     * @return true if message was read, false if no message available
     */
    virtual bool read(CANMessage& msg) = 0;
    
    /**
     * @brief Send a CAN message
     * @param msg Message to send
     * @return true if message was sent successfully
     */
    virtual bool write(const CANMessage& msg) = 0;
    
    /**
     * @brief Check if CAN interface is ready
     * @return true if ready for communication
     */
    virtual bool isReady() const = 0;
};

// /**
//  * @brief Real CAN implementation using mbed CAN
//  */
class MbedCAN : public ICAN {
private:
    CAN& _can;
    
public:
    explicit MbedCAN(CAN& can) : _can(can) {}
    
    bool read(CANMessage& msg) override {
        return _can.read(msg);
    }
    
    bool write(const CANMessage& msg) override {
        return _can.write(msg);
    }
    
    bool isReady() const override {
        return true; // mbed CAN is always ready after construction
    }
};

/**
 * @brief Mock CAN implementation for testing on STM32
 */
class MockCAN : public ICAN {
private:
    static constexpr size_t MAX_MESSAGES = 32;
    CANMessage _incomingMessages[MAX_MESSAGES];
    CANMessage _sentMessages[MAX_MESSAGES];
    size_t _incomingCount = 0;
    size_t _sentCount = 0;
    size_t _messageIndex = 0;
    bool _isReady = true;
    
public:
    // Test setup methods
    void injectMessage(const CANMessage& msg) {
        if (_incomingCount < MAX_MESSAGES) {
            _incomingMessages[_incomingCount++] = msg;
        }
    }
    
    void setReady(bool ready) { _isReady = ready; }
    
    // Verification methods
    size_t getSentMessageCount() const { return _sentCount; }
    const CANMessage& getSentMessage(size_t index) const { return _sentMessages[index]; }
    
    void clearSentMessages() { _sentCount = 0; }
    void clearIncomingMessages() { 
        _incomingCount = 0; 
        _messageIndex = 0; 
    }
    
    // ICAN implementation
    bool read(CANMessage& msg) override {
        if (_messageIndex >= _incomingCount) {
            return false;
        }
        msg = _incomingMessages[_messageIndex++];
        return true;
    }
    
    bool write(const CANMessage& msg) override {
        if (!_isReady || _sentCount >= MAX_MESSAGES) return false;
        _sentMessages[_sentCount++] = msg;
        return true;
    }
    
    bool isReady() const override {
        return _isReady;
    }
};

#endif // CAN_INTERFACE_HPP 