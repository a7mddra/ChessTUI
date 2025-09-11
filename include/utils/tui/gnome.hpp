#pragma once

/**
 * @file loader.hpp
 * @brief Provides optional utilities for GNOME Terminal integration.
 *
 * This module contains functions to enhance the application's user experience
 * when run within a GNOME desktop environment. It can auto-detect GNOME Terminal,
 * manage custom terminal profiles, and spawn new terminals for logging. If the
 * environment is not GNOME, these functions will gracefully do nothing or fall
 * back to standard output.
 *
 * Author: a7mddra
 */

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

/**
 * @namespace gnome
 * @brief A namespace for GNOME-specific terminal utilities.
 */
namespace gnome
{
    /**
     * @brief Checks for a GNOME environment and relaunches the app in a custom profile.
     * @param argc Argument count from main().
     * @param argv Argument vector from main().
     *
     * If `gnome-terminal` and `dconf` are detected, this function ensures a
     * "Chess" profile exists (creating it if necessary) and relaunches the
     * application in a new terminal using that profile. The original process then
     * exits.
     *
     * If not in a GNOME environment, or if the app was already relaunched (via
     * the `--chess-run` flag), this function does nothing.
     */
    void loadProfile(int argc, char **argv);
}
