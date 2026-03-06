#pragma once

#include <memory>
#include <vector>

#include "Types.h"

namespace chess {

class Board;

class Piece {
public:
    Piece(PieceColor color, PieceType type) : color_(color), type_(type) {}
    virtual ~Piece() = default;

    PieceColor color() const { return color_; }
    PieceType type() const { return type_; }

    bool hasMoved() const { return hasMoved_; }
    void markMoved() { hasMoved_ = true; }
    void resetMoved() { hasMoved_ = false; }

    virtual std::vector<Move> getPseudoLegalMoves(const Board& board, Position from) const = 0;
    virtual std::unique_ptr<Piece> clone() const = 0;

private:
    PieceColor color_;
    PieceType type_;
    bool hasMoved_{false};
};

class Pawn : public Piece {
public:
    explicit Pawn(PieceColor color) : Piece(color, PieceType::Pawn) {}
    std::vector<Move> getPseudoLegalMoves(const Board& board, Position from) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Pawn>(*this); }
};

class Rook : public Piece {
public:
    explicit Rook(PieceColor color) : Piece(color, PieceType::Rook) {}
    std::vector<Move> getPseudoLegalMoves(const Board& board, Position from) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Rook>(*this); }
};

class Knight : public Piece {
public:
    explicit Knight(PieceColor color) : Piece(color, PieceType::Knight) {}
    std::vector<Move> getPseudoLegalMoves(const Board& board, Position from) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Knight>(*this); }
};

class Bishop : public Piece {
public:
    explicit Bishop(PieceColor color) : Piece(color, PieceType::Bishop) {}
    std::vector<Move> getPseudoLegalMoves(const Board& board, Position from) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Bishop>(*this); }
};

class Queen : public Piece {
public:
    explicit Queen(PieceColor color) : Piece(color, PieceType::Queen) {}
    std::vector<Move> getPseudoLegalMoves(const Board& board, Position from) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<Queen>(*this); }
};

class King : public Piece {
public:
    explicit King(PieceColor color) : Piece(color, PieceType::King) {}
    std::vector<Move> getPseudoLegalMoves(const Board& board, Position from) const override;
    std::unique_ptr<Piece> clone() const override { return std::make_unique<King>(*this); }
};

}  // namespace chess
