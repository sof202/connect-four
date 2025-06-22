#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <array>
#include <string>
#include <utility>

namespace MessageType {
enum Type : std::uint8_t {
   move,
   requestInput,
   end,
   info,
   maxMessageType,
};
using namespace std::string_view_literals;
constexpr std::array message_type_names{
    "move"sv,
    "requestInput"sv,
    "end"sv,
    "info"sv,
};
static_assert(message_type_names.size() ==
              static_cast<std::size_t>(maxMessageType));
constexpr auto getTypeName(const Type& type) -> std::string_view {
   return message_type_names.at(static_cast<std::size_t>(type));
}
constexpr auto getTypeFromName(std::string_view type_name) -> Type {
   for (std::size_t idx{0}; idx < maxMessageType; ++idx) {
      if (message_type_names.at(idx) == type_name)
         return static_cast<Type>(idx);
   }
   return info;
}
}  // namespace MessageType

class Message {
  public:
   Message(MessageType::Type message_type, std::string message_text) :
       m_message_type{message_type},
       m_message_text{std::move(message_text)} {}

   static auto fromString(const std::string& full_message) {
      std::size_t separator_position{full_message.find(' ')};
      std::string message_text{full_message.substr(separator_position + 1)};
      MessageType::Type message_type{MessageType::getTypeFromName(
          full_message.substr(0, separator_position))};
      return Message{message_type, message_text};
   }

   [[nodiscard]] auto messageText() const -> std::string {
      return m_message_text;
   }
   [[nodiscard]] auto messageType() const -> MessageType::Type {
      return m_message_type;
   }
   [[nodiscard]] auto asString() const -> std::string {
      return std::string(MessageType::getTypeName(m_message_type)) + " " +
             m_message_text;
   }

  private:
   MessageType::Type m_message_type;
   std::string m_message_text;
};

#endif  // MESSAGE_H_
