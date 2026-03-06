#pragma once

#include <optional>

namespace chess {

enum class PieceColor { White, Black };
enum class PieceType { Pawn, Rook, Knight, Bishop, Queen, King };

struct Position {
    int row{};
    int col{};

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

struct Move {
    Position from;
    Position to;
    bool isCapture{false};
    bool isCastling{false};
    bool isEnPassant{false};
    std::optional<PieceType> promotion;

    bool operator==(const Move& other) const {
        return from == other.from && to == other.to && promotion == other.promotion;
    }
};

inline PieceColor oppositeColor(PieceColor color) {
    return color == PieceColor::White ? PieceColor::Black : PieceColor::White;
}

}  // namespace chess
