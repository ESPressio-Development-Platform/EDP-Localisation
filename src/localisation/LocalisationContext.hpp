#pragma once

#include <cstdint>

#include "LanguageIdentifierView.hpp"

namespace ESPressio::Localisation {

    /// Mutually exclusive outcome of validating one caller-owned Localisation context.
    enum class LocalisationContextValidationStatus : std::uint8_t {
        Succeeded = 0U,
        InvalidRequestedLanguage = 1U,
        InvalidTerminalLanguage = 2U
    };


    /// Caller-owned language policy supplied independently to each Localisation operation.
    struct LocalisationContext final {

        /// Canonical language requested for the current operation.
        LanguageIdentifierView RequestedLanguage;

        /// Canonical unique terminal fallback language for the ContractFamily.
        LanguageIdentifierView TerminalLanguage;

    };


    /// Result of validating one LocalisationContext's language identities.
    struct LocalisationContextValidationResult final {

        /// Validation outcome.
        LocalisationContextValidationStatus Status;

    };


    /// Validates the two canonical language identities carried by Context.
    [[nodiscard]] constexpr LocalisationContextValidationResult ValidateLocalisationContext(
        const LocalisationContext& Context
    ) noexcept {
        const auto Requested = LanguageIdentifierView::Validate(
            Context.RequestedLanguage.Data(),
            Context.RequestedLanguage.Length()
        );

        if (!Requested.IsValuePresent) {
            return {
                LocalisationContextValidationStatus::InvalidRequestedLanguage
            };
        }

        const auto Terminal = LanguageIdentifierView::Validate(
            Context.TerminalLanguage.Data(),
            Context.TerminalLanguage.Length()
        );

        if (!Terminal.IsValuePresent) {
            return {
                LocalisationContextValidationStatus::InvalidTerminalLanguage
            };
        }

        return {
            LocalisationContextValidationStatus::Succeeded
        };
    }

} // ESPressio::Localisation
