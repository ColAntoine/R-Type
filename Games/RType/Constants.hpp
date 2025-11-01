/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Sun, Oct, 2025                                                     *
 * Title           - RTYPE                                                              *
 * Description     -                                                                    *
 *     Constants                                                                        *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *         ░        ░       ░░        ░        ░        ░░      ░░  ░░░░  ░             *
 *         ▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒  ▒▒▒▒  ▒             *
 *         ▓      ▓▓▓       ▓▓▓▓▓  ▓▓▓▓▓▓▓  ▓▓▓▓      ▓▓▓  ▓▓▓▓▓▓▓        ▓             *
 *         █  ███████  ██████████  ███████  ████  ███████  ████  █  ████  █             *
 *         █        █  ███████        ████  ████        ██      ██  ████  █             *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#ifndef INCLUDED_CONSTANTS_HPP
    #define INCLUDED_CONSTANTS_HPP

#include <iostream>
#include <string>

// ====================================WIN======================================
#define SCREEN_WIDTH        1920
#define SCREEN_HEIGHT       1080
// =============================================================================

// ==================================GRAVITY====================================
#define GRAVITY             500.f
// =============================================================================

// ===================================BALLS=====================================
#define MAX_BALLS           3
// =============================================================================

// ===================================COLLISION=================================
#define COLLISION_WIDTH     40.f
#define COLLISION_HEIGHT    40.f
// =============================================================================

// ====================================PLAYER===================================
#define PLAYER_SPAWN_X      100.f
#define PLAYER_SPAWN_Y      200.f
// =============================================================================

// ==================================REPO ROOT==================================
#ifndef REPO_ROOT
    #define REPO_ROOT "/home/sum/Documents/TEK3/RTYPE/RTYPE"
#endif
#define RTYPE_REPO_ROOT REPO_ROOT
// =============================================================================

// ===================================LibECS====================================
#define LIBECS_PATH        (std::string(RTYPE_REPO_ROOT) + "/ECS/build/lib/libECS.so")
// =============================================================================

// ====================================Assets===================================
#define RTYPE_PATH_ASSETS   (std::string(RTYPE_REPO_ROOT) + "/Games/RType/Assets/")
// =============================================================================

// ====================================Assets===================================
#define BOSS_BASE_FIRERATE      1.f
#define BOSS_BASE_DAMAGE        5.f
#define BOSS_BASE_PROJ_SPEED    300.f
// =============================================================================

// ====================================Config===================================
#define RTYPE_PATH_FILE_CONFIG   (std::string(RTYPE_REPO_ROOT) + "/Games/RType/.rtype/GameConfig.cfg")
// =============================================================================
#endif

/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * MIT License                                                                          *
 * Copyright (c) 2025 paul1.emeriau                                                     *
 *                                                                                      *
 * Permission is hereby granted, free of charge, to any person obtaining a copy         *
 * of this software and associated documentation files (the "Software"), to deal        *
 * in the Software without restriction, including without limitation the rights         *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            *
 * copies of the Software, and to permit persons to whom the Software is                *
 * furnished to do so, subject to the following conditions:                             *
 *                                                                                      *
 * The above copyright notice and this permission notice shall be included in all       *
 * copies or substantial portions of the Software.                                      *
 *                                                                                      *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        *
 * SOFTWARE.                                                                            *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */
