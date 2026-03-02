#include "ChatManager.h"
#include <sstream>

void ChatManager::Push(ChatMessageType type, ClientID senderID,
                       const std::string &senderName, const std::string &text)
{
    ChatEntry entry;
    entry.type = type;
    entry.senderID = senderID;
    entry.senderName = senderName;
    entry.text = text;
    // timestamp could be set here with a wall clock if needed

    m_history.push_back(std::move(entry));
    ++m_newCount;

    while (m_history.size() > m_maxHistory)
        m_history.pop_front();
}

/// Escape a string for embedding inside a JSON string literal.
static std::string EscapeJSON(const std::string &s)
{
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s)
    {
        switch (c)
        {
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            if (static_cast<unsigned char>(c) < 0x20)
            {
                // Control character – encode as \u00XX
                char buf[8];
                snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                out += buf;
            }
            else
            {
                out += c;
            }
            break;
        }
    }
    return out;
}

static const char *ChatTypeToString(ChatMessageType type)
{
    switch (type)
    {
    case ChatMessageType::System:
        return "system";
    case ChatMessageType::Public:
        return "public";
    case ChatMessageType::Whisper:
        return "whisper";
    default:
        return "unknown";
    }
}

std::string ChatManager::EntryToJSON(const ChatEntry &entry)
{
    std::ostringstream ss;
    ss << "{\"type\":\"" << ChatTypeToString(entry.type)
       << "\",\"senderID\":" << entry.senderID
       << ",\"senderName\":\"" << EscapeJSON(entry.senderName)
       << "\",\"text\":\"" << EscapeJSON(entry.text)
       << "\"}";
    return ss.str();
}

std::string ChatManager::ToJSON(size_t count) const
{
    std::ostringstream ss;
    ss << "[";

    size_t total = m_history.size();
    size_t start = 0;
    if (count > 0 && count < total)
        start = total - count;

    bool first = true;
    for (size_t i = start; i < total; ++i)
    {
        if (!first)
            ss << ",";
        ss << EntryToJSON(m_history[i]);
        first = false;
    }

    ss << "]";
    return ss.str();
}
