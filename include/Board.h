#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Piece.h"

namespace chess {

struct PromotionState {
    Position position;
    PieceColor color;
};

class Board {
public:
    Board();
    Board(const Board& other);
    Board& operator=(const Board& other);

    void reset();

    const Piece* getPiece(Position position) const;
    Piece* getPiece(Position position);

    PieceColor currentTurn() const { return currentTurn_; }
    bool inBounds(Position position) const;
    bool isEmpty(Position position) const;
    bool hasEnemyPiece(Position position, PieceColor selfColor) const;

    std::vector<Move> getLegalMoves(Position from) const;
    std::vector<Move> getAllLegalMoves(PieceColor color) const;

    bool isInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool isStalemate(PieceColor color) const;

    bool tryMove(const Move& move, std::string& message);

    bool hasPendingPromotion() const { return pendingPromotion_.has_value(); }
    const std::optional<PromotionState>& pendingPromotion() const { return pendingPromotion_; }
    bool promotePawn(PieceType type, std::string& message);

    std::optional<Position> lastMoveFrom() const { return lastMoveFrom_; }
    std::optional<Position> lastMoveTo() const { return lastMoveTo_; }

private:
    using Grid = std::array<std::array<std::unique_ptr<Piece>, 8>, 8>;

    Grid grid_;
    PieceColor currentTurn_{PieceColor::White};
    std::optional<Move> lastMove_;
    std::optional<Position> lastMoveFrom_;
    std::optional<Position> lastMoveTo_;
    std::optional<PromotionState> pendingPromotion_;

    void setupPieces();
    std::vector<Move> getPseudoMoves(Position from) const;

    bool isSquareAttacked(Position square, PieceColor byColor) const;
    Position findKing(PieceColor color) const;

    void executeMoveUnchecked(const Move& move);
};

}  // namespace chess
