#pragma once

#include <cstddef>
#include <cstdint>

namespace ESPressio::Localisation::Detail::Edpl {

    /// Current persisted section identifiers for the release-owned EDPL format.
    enum class SectionType : std::uint8_t {
        LanguageMetadata = 1U,
        LanguageDisplayNames = 2U,
        GeneralStrings = 3U,
        TypeSchema = 4U,
        Utf8Payload = 5U
    };


    /// Number of fixed bytes preceding the section directory.
    inline constexpr std::size_t FixedPreambleBytes = 22U;

    /// Number of bytes in one section-directory entry.
    inline constexpr std::size_t SectionDirectoryEntryBytes = 12U;

    /// Offset of the persisted whole-file CRC32C field inside the fixed preamble.
    inline constexpr std::size_t FileCrc32cOffset = 18U;

    /// Number of bytes occupied by the persisted whole-file CRC32C field.
    inline constexpr std::size_t FileCrc32cBytes = 4U;

    /// Number of required current-format sections.
    inline constexpr std::size_t RequiredSectionCount = 5U;

    /// Fixed Language Metadata bytes preceding language and parent tag payloads.
    inline constexpr std::size_t LanguageMetadataFixedBytes = 20U;

    /// Fixed Language Display-Name section header size.
    inline constexpr std::size_t LanguageDisplayHeaderBytes = 4U;

    /// Fixed Language Display-Name record size.
    inline constexpr std::size_t LanguageDisplayEntryBytes = 16U;

    /// Fixed General Strings section header size.
    inline constexpr std::size_t GeneralStringsHeaderBytes = 28U;

    /// Fixed Type Schema section header size.
    inline constexpr std::size_t TypeSchemaHeaderBytes = 20U;

    /// Fixed UTF-8 Payload section header size.
    inline constexpr std::size_t PayloadHeaderBytes = 8U;

    /// Current section schema version used by every V1 section.
    inline constexpr std::uint8_t SectionVersion = 1U;

    /// Language Metadata flag selecting the unique terminal language.
    inline constexpr std::uint8_t LanguageTerminalFlag = 0x01U;

    /// Type/Field representation flag indicating a present Name.
    inline constexpr std::uint8_t NamePresentFlag = 0x01U;

    /// Type/Field representation flag indicating a present Description.
    inline constexpr std::uint8_t DescriptionPresentFlag = 0x02U;

} // ESPressio::Localisation::Detail::Edpl
