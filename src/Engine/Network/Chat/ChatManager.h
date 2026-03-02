#pragma once
#include "Engine/Network/Protocol/Messages.h"
#include <string>
#include <vector>
#include <cstdint>
#include <deque>

/// A single chat entry stored in the local history.
struct ChatEntry
{
    ChatMessageType type = ChatMessageType::Public;
    ClientID senderID = INVALID_CLIENT_ID;
    std::string senderName;
    std::string text;
    double timestamp = 0.0; // game-time or wall-clock when received
};

/// Client-side chat message store & helper.
/// Keeps a rolling history (ring buffer style) and provides a JSON-like
/// string for pushing to the Vue UI layer.
class ChatManager
{
public:
    ChatManager() = default;

    /// Push a new message into the history.
    void Push(ChatMessageType type, ClientID senderID,
              const std::string &senderName, const std::string &text);

    /// Get the full message history (most recent last).
    const std::deque<ChatEntry> &GetHistory() const { return m_history; }

    /// Number of messages received since last call to ResetNewCount().
    int GetNewMessageCount() const { return m_newCount; }
    void ResetNewCount() { m_newCount = 0; }

    /// Serialize the latest `count` messages to a JSON array string
    /// suitable for passing to Vue via ExecuteScript.
    std::string ToJSON(size_t count = 0) const;

    /// Serialize a single ChatEntry to a JSON object string.
    static std::string EntryToJSON(const ChatEntry &entry);

    /// Maximum messages kept in history.
    void SetMaxHistory(size_t max) { m_maxHistory = max; }

private:
    std::deque<ChatEntry> m_history;
    size_t m_maxHistory = 100;
    int m_newCount = 0;
};
