#include "Board.h"

#include <algorithm>
#include <stdexcept>

namespace chess {

Board::Board() {
    reset();
}

Board::Board(const Board& other) {
    *this = other;
}

Board& Board::operator=(const Board& other) {
    if (this == &other) {
        return *this;
    }

    currentTurn_ = other.currentTurn_;
    lastMove_ = other.lastMove_;
    lastMoveFrom_ = other.lastMoveFrom_;
    lastMoveTo_ = other.lastMoveTo_;
    pendingPromotion_ = other.pendingPromotion_;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (other.grid_[row][col]) {
                grid_[row][col] = other.grid_[row][col]->clone();
            } else {
                grid_[row][col].reset();
            }
        }
    }

    return *this;
}

void Board::reset() {
    for (auto& row : grid_) {
        for (auto& piece : row) {
            piece.reset();
        }
    }

    currentTurn_ = PieceColor::White;
    lastMove_.reset();
    lastMoveFrom_.reset();
    lastMoveTo_.reset();
    pendingPromotion_.reset();
    setupPieces();
}

void Board::setupPieces() {
    for (int col = 0; col < 8; ++col) {
        grid_[1][col] = std::make_unique<Pawn>(PieceColor::Black);
        grid_[6][col] = std::make_unique<Pawn>(PieceColor::White);
    }

    grid_[0][0] = std::make_unique<Rook>(PieceColor::Black);
    grid_[0][7] = std::make_unique<Rook>(PieceColor::Black);
    grid_[7][0] = std::make_unique<Rook>(PieceColor::White);
    grid_[7][7] = std::make_unique<Rook>(PieceColor::White);

    grid_[0][1] = std::make_unique<Knight>(PieceColor::Black);
    grid_[0][6] = std::make_unique<Knight>(PieceColor::Black);
    grid_[7][1] = std::make_unique<Knight>(PieceColor::White);
    grid_[7][6] = std::make_unique<Knight>(PieceColor::White);

    grid_[0][2] = std::make_unique<Bishop>(PieceColor::Black);
    grid_[0][5] = std::make_unique<Bishop>(PieceColor::Black);
    grid_[7][2] = std::make_unique<Bishop>(PieceColor::White);
    grid_[7][5] = std::make_unique<Bishop>(PieceColor::White);

    grid_[0][3] = std::make_unique<Queen>(PieceColor::Black);
    grid_[7][3] = std::make_unique<Queen>(PieceColor::White);

    grid_[0][4] = std::make_unique<King>(PieceColor::Black);
    grid_[7][4] = std::make_unique<King>(PieceColor::White);
}

const Piece* Board::getPiece(Position position) const {
    if (!inBounds(position)) {
        return nullptr;
    }
    return grid_[position.row][position.col].get();
}

Piece* Board::getPiece(Position position) {
    if (!inBounds(position)) {
        return nullptr;
    }
    return grid_[position.row][position.col].get();
}

bool Board::inBounds(Position position) const {
    return position.row >= 0 && position.row < 8 && position.col >= 0 && position.col < 8;
}

bool Board::isEmpty(Position position) const {
    return inBounds(position) && getPiece(position) == nullptr;
}

bool Board::hasEnemyPiece(Position position, PieceColor selfColor) const {
    const Piece* piece = getPiece(position);
    return piece != nullptr && piece->color() != selfColor;
}

std::vector<Move> Board::getPseudoMoves(Position from) const {
    const Piece* piece = getPiece(from);
    if (!piece) {
        return {};
    }

    std::vector<Move> moves = piece->getPseudoLegalMoves(*this, from);

    if (piece->type() == PieceType::King && !piece->hasMoved() && !isInCheck(piece->color())) {
        const int row = from.row;

        Position kingsideRookPos{row, 7};
        const Piece* kingsideRook = getPiece(kingsideRookPos);
        if (kingsideRook && kingsideRook->type() == PieceType::Rook && kingsideRook->color() == piece->color() &&
            !kingsideRook->hasMoved() && isEmpty(Position{row, 5}) && isEmpty(Position{row, 6}) &&
            !isSquareAttacked(Position{row, 5}, oppositeColor(piece->color())) &&
            !isSquareAttacked(Position{row, 6}, oppositeColor(piece->color()))) {
            moves.push_back(Move{from, Position{row, 6}, false, true});
        }

        Position queensideRookPos{row, 0};
        const Piece* queensideRook = getPiece(queensideRookPos);
        if (queensideRook && queensideRook->type() == PieceType::Rook && queensideRook->color() == piece->color() &&
            !queensideRook->hasMoved() && isEmpty(Position{row, 1}) && isEmpty(Position{row, 2}) &&
            isEmpty(Position{row, 3}) && !isSquareAttacked(Position{row, 3}, oppositeColor(piece->color())) &&
            !isSquareAttacked(Position{row, 2}, oppositeColor(piece->color()))) {
            moves.push_back(Move{from, Position{row, 2}, false, true});
        }
    }

    return moves;
}

std::vector<Move> Board::getLegalMoves(Position from) const {
    const Piece* piece = getPiece(from);
    if (!piece || piece->color() != currentTurn_ || pendingPromotion_) {
        return {};
    }

    std::vector<Move> legalMoves;
    for (const Move& move : getPseudoMoves(from)) {
        Board simulated(*this);
        simulated.executeMoveUnchecked(move);
        if (!simulated.isInCheck(piece->color())) {
            legalMoves.push_back(move);
        }
    }

    return legalMoves;
}

std::vector<Move> Board::getAllLegalMoves(PieceColor color) const {
    std::vector<Move> moves;
    if (pendingPromotion_) {
        return moves;
    }

    Board temp(*this);
    temp.currentTurn_ = color;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece* piece = getPiece(Position{row, col});
            if (piece && piece->color() == color) {
                auto pieceMoves = temp.getLegalMoves(Position{row, col});
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    return moves;
}

bool Board::isSquareAttacked(Position square, PieceColor byColor) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece* piece = getPiece(Position{row, col});
            if (!piece || piece->color() != byColor) {
                continue;
            }

            Position from{row, col};
            if (piece->type() == PieceType::Pawn) {
                int direction = byColor == PieceColor::White ? -1 : 1;
                if (square.row == row + direction && (square.col == col - 1 || square.col == col + 1)) {
                    return true;
                }
                continue;
            }

            auto attacks = piece->getPseudoLegalMoves(*this, from);
            if (std::any_of(attacks.begin(), attacks.end(), [&](const Move& move) { return move.to == square; })) {
                return true;
            }
        }
    }
    return false;
}

Position Board::findKing(PieceColor color) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece* piece = getPiece(Position{row, col});
            if (piece && piece->type() == PieceType::King && piece->color() == color) {
                return Position{row, col};
            }
        }
    }
    throw std::runtime_error("King not found");
}

bool Board::isInCheck(PieceColor color) const {
    if (pendingPromotion_) {
        return false;
    }
    Position kingPosition = findKing(color);
    return isSquareAttacked(kingPosition, oppositeColor(color));
}

bool Board::isCheckmate(PieceColor color) const {
    if (!isInCheck(color)) {
        return false;
    }
    return getAllLegalMoves(color).empty();
}

bool Board::isStalemate(PieceColor color) const {
    if (isInCheck(color)) {
        return false;
    }
    return getAllLegalMoves(color).empty();
}

void Board::executeMoveUnchecked(const Move& move) {
    auto& movingPiece = grid_[move.from.row][move.from.col];

    if (move.isCastling && movingPiece && movingPiece->type() == PieceType::King) {
        if (move.to.col == 6) {
            grid_[move.to.row][5] = std::move(grid_[move.to.row][7]);
            if (grid_[move.to.row][5]) {
                grid_[move.to.row][5]->markMoved();
            }
        } else if (move.to.col == 2) {
            grid_[move.to.row][3] = std::move(grid_[move.to.row][0]);
            if (grid_[move.to.row][3]) {
                grid_[move.to.row][3]->markMoved();
            }
        }
    }

    grid_[move.to.row][move.to.col] = std::move(movingPiece);
    if (grid_[move.to.row][move.to.col]) {
        grid_[move.to.row][move.to.col]->markMoved();
    }

    lastMove_ = move;
    lastMoveFrom_ = move.from;
    lastMoveTo_ = move.to;

    if (Piece* movedPiece = getPiece(move.to); movedPiece && movedPiece->type() == PieceType::Pawn) {
        if ((movedPiece->color() == PieceColor::White && move.to.row == 0) ||
            (movedPiece->color() == PieceColor::Black && move.to.row == 7)) {
            pendingPromotion_ = PromotionState{move.to, movedPiece->color()};
        }
    }
}

bool Board::tryMove(const Move& move, std::string& message) {
    if (pendingPromotion_) {
        message = "Escolha uma promoção antes de continuar.";
        return false;
    }

    auto legalMoves = getLegalMoves(move.from);
    auto it = std::find_if(legalMoves.begin(), legalMoves.end(), [&](const Move& legal) { return legal.to == move.to; });

    if (it == legalMoves.end()) {
        message = "Jogada inválida.";
        return false;
    }

    executeMoveUnchecked(*it);

    if (!pendingPromotion_) {
        currentTurn_ = oppositeColor(currentTurn_);
    }

    message = "Jogada realizada.";
    return true;
}

bool Board::promotePawn(PieceType type, std::string& message) {
    if (!pendingPromotion_) {
        message = "Nenhuma promoção pendente.";
        return false;
    }

    if (type == PieceType::King || type == PieceType::Pawn) {
        message = "Promoção inválida.";
        return false;
    }

    const Position position = pendingPromotion_->position;
    const PieceColor color = pendingPromotion_->color;

    switch (type) {
        case PieceType::Queen:
            grid_[position.row][position.col] = std::make_unique<Queen>(color);
            break;
        case PieceType::Rook:
            grid_[position.row][position.col] = std::make_unique<Rook>(color);
            break;
        case PieceType::Bishop:
            grid_[position.row][position.col] = std::make_unique<Bishop>(color);
            break;
        case PieceType::Knight:
            grid_[position.row][position.col] = std::make_unique<Knight>(color);
            break;
        default:
            break;
    }

    if (grid_[position.row][position.col]) {
        grid_[position.row][position.col]->markMoved();
    }

    pendingPromotion_.reset();
    currentTurn_ = oppositeColor(currentTurn_);

    message = "Peão promovido.";
    return true;
}

}  // namespace chess
