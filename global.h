#pragma once

enum MessageType
{
    kMessage,
    kStatus,
    kInfo,
    kUsername,
    kInvalidUsername,
    kUserAdded,
    kUserRemoved,
    kHello,
    kInvalidHello
};

enum Status
{
    kAvailable,
    kMovedAway,
    kDoNotDisturb
};

constexpr char CIPHER_NAME[] = "DHE-PSK-AES256-GCM-SHA384";
