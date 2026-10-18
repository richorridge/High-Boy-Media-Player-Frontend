#!/bin/bash

# Copyright (c) 2025 HIGH CODE LLC
#
# TentacleOS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TentacleOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TentacleOS. If not, see <https://www.gnu.org/licenses/>.


# ==============================================================================
# TentacleOS Developer Environment Setup
# ==============================================================================
# Run this once after cloning the repository.

GREEN='\033[0;32m'
NC='\033[0m'

echo "Setting up TentacleOS development environment..."

git config core.hooksPath .githooks
echo -e "  Git hooks path set to ${GREEN}.githooks/${NC}"

echo -e "${GREEN}Setup complete!${NC}"
