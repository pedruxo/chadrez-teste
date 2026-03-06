#include "Piece.h"

#include <array>

#include "Board.h"

namespace chess {

namespace {

void addSlidingMoves(const Board& board, Position from, PieceColor color,
                    const std::vector<std::pair<int, int>>& directions,
                    std::vector<Move>& moves) {
    for (const auto& [dr, dc] : directions) {
        Position current{from.row + dr, from.col + dc};
        while (board.inBounds(current)) {
            if (board.isEmpty(current)) {
                moves.push_back(Move{from, current, false});
            } else {
                if (board.hasEnemyPiece(current, color)) {
                    moves.push_back(Move{from, current, true});
                }
                break;
            }
            current = Position{current.row + dr, current.col + dc};
        }
    }
}

}  // namespace

std::vector<Move> Pawn::getPseudoLegalMoves(const Board& board, Position from) const {
    std::vector<Move> moves;

    const int direction = color() == PieceColor::White ? -1 : 1;
    const int startRow = color() == PieceColor::White ? 6 : 1;

    Position oneStep{from.row + direction, from.col};
    if (board.inBounds(oneStep) && board.isEmpty(oneStep)) {
        moves.push_back(Move{from, oneStep, false});

        Position twoStep{from.row + 2 * direction, from.col};
        if (from.row == startRow && board.inBounds(twoStep) && board.isEmpty(twoStep)) {
            moves.push_back(Move{from, twoStep, false});
        }
    }

    for (int dc : {-1, 1}) {
        Position capturePos{from.row + direction, from.col + dc};
        if (board.inBounds(capturePos) && board.hasEnemyPiece(capturePos, color())) {
            moves.push_back(Move{from, capturePos, true});
        }
    }

    return moves;
}

std::vector<Move> Rook::getPseudoLegalMoves(const Board& board, Position from) const {
    std::vector<Move> moves;
    addSlidingMoves(board, from, color(), {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}, moves);
    return moves;
}

std::vector<Move> Knight::getPseudoLegalMoves(const Board& board, Position from) const {
    std::vector<Move> moves;
    constexpr std::array<std::pair<int, int>, 8> offsets{{
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2},
    }};

    for (const auto& [dr, dc] : offsets) {
        Position target{from.row + dr, from.col + dc};
        if (!board.inBounds(target)) {
            continue;
        }
        if (board.isEmpty(target)) {
            moves.push_back(Move{from, target, false});
        } else if (board.hasEnemyPiece(target, color())) {
            moves.push_back(Move{from, target, true});
        }
    }

    return moves;
}

std::vector<Move> Bishop::getPseudoLegalMoves(const Board& board, Position from) const {
    std::vector<Move> moves;
    addSlidingMoves(board, from, color(), {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}, moves);
    return moves;
}

std::vector<Move> Queen::getPseudoLegalMoves(const Board& board, Position from) const {
    std::vector<Move> moves;
    addSlidingMoves(board, from, color(),
                    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}, moves);
    return moves;
}

std::vector<Move> King::getPseudoLegalMoves(const Board& board, Position from) const {
    std::vector<Move> moves;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            Position target{from.row + dr, from.col + dc};
            if (!board.inBounds(target)) {
                continue;
            }
            if (board.isEmpty(target)) {
                moves.push_back(Move{from, target, false});
            } else if (board.hasEnemyPiece(target, color())) {
                moves.push_back(Move{from, target, true});
            }
        }
    }
    return moves;
}

}  // namespace chess
