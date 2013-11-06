/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef TUPLE_H
#define TUPLE_H

template<typename T>
class Tuple
{
public:
    Tuple(void)
    {
        x = y = 0;
    }


    Tuple(T const& kX, T const& kY)
    {
        x = kX;
        y = kY;
    }

    Tuple(Tuple<T> const& kTuple)
    {
        x = kTuple.x;
        y = kTuple.y;
    }

    Tuple(float const& kValue)
    {
        x = y = kValue;
    }

    Tuple AddX(T const& kSummand)
    {
        return Tuple<T>(x + kSummand, y);
    }

    Tuple SubX(T const& kSub)
    {
        return Tuple<T>(x - kSub, y);
    }

    Tuple AddY(T const& kSummand)
    {
        return Tuple<T>(x, y + kSummand);
    }

    Tuple SubY(T const& kSub)
    {
        return Tuple<T>(x, y - kSub);
    }

// algebraic operations
    Tuple operator+(T const& kSummand)
    {
        return Tuple<T>(x + kSummand, y + kSummand);
    }

    Tuple operator-(T const& kSub)
    {
        return Tuple<T>(x - kSub, y - kSub);
    }

    Tuple operator*(T const& kFactor)
    {
        return Tuple<T>(x*kFactor, y*kFactor);
    }

    Tuple operator/(T const& kDivisor)
    {
        return Tuple<T>( x/kDivisor, y/kDivisor);
    }


    Tuple operator+(Tuple const& kTuple) const
    {
        return Tuple<T>(x + kTuple.x, y + kTuple.y);
    }

    Tuple operator-(Tuple const& kTuple) const
    {
        return Tuple<T>(x - kTuple.x, y - kTuple.y);
    }

    Tuple operator*(Tuple const& kTuple)
    {
        return Tuple<T>(x*kTuple.x, y*kTuple.y);
    }

    Tuple operator/(Tuple const& kTuple)
    {
        return Tuple<T>(x/kTuple.x, y/kTuple.y);
    }

    Tuple& operator+=(T const& kSummand)
    {
        x += kSummand;
        y += kSummand;
        return *this;
    }

    Tuple& operator-=(T const& kSub)
    {
        x -= kSub;
        y -= kSub;

        return *this;
    }

    Tuple& operator*=(T const& kFactor)
    {
        x *= kFactor;
        y *= kFactor;
        return *this;
    }

    Tuple& operator/=(T const& kDivisor)
    {
        x /= kDivisor;
        y /= kDivisor;
        return *this;
    }


    Tuple& operator+=(Tuple const& kTuple)
    {
        x += kTuple.x;
        y += kTuple.y;
        return *this;
    }

    Tuple& operator-=(Tuple const& kTuple)
    {
        x -= kTuple.x;
        y -= kTuple.y;
        return *this;
    }

    Tuple& operator*=(Tuple const& kTuple)
    {
        x *= kTuple.x;
        y *= kTuple.y;
        return *this;
    }

    Tuple& operator/=(Tuple const& kTuple)
    {
        x /= kTuple.x;
        y /= kTuple.y;
        return *this;
    }


    Tuple& operator=(Tuple const& kTuple)
    {
        x = kTuple.x;
        y = kTuple.y;
        return *this;
    }

    Tuple& operator=(T const& kValue)
    {
        x = y = kValue;
        return *this;
    }

    /*
    Tuple& operator++( T const& kValue )
    {
        x++;
        y++;

        return( *this );
    }

    Tuple& operator--( T const& kValue )
    {
        x--;
        y--;

        return( *this );
    }
    */

// Comparision

    bool operator<(T const& kComp) const
    {
        return ((x*x + y*y) < kComp*kComp);
    }

    bool operator<=(T const& kComp) const
    {
        return ((x*x + y*y) <= kComp*kComp);
    }

    bool operator>(T const& kComp) const
    {
        return ((x*x + y*y) > kComp*kComp);
    }

    bool operator>=(T const& kComp) const
    {
        return ((x*x + y*y) >= kComp*kComp);
    }

    bool operator==(T const& kComp) const
    {
        return (x == kComp && y == kComp);
    }

    bool operator!=(T const& kComp) const
    {
        return (x != kComp || y != kComp);
    }

    bool operator<(Tuple const& kTuple) const
    {
        return (x*x + y*y < kTuple.x*kTuple.x + kTuple.y*kTuple.y);
    }

    bool operator<=(Tuple const& kTuple) const
    {
        return (x*x + y*y <= kTuple.x*kTuple.x + kTuple.y*kTuple.y);
    }

    bool operator>(Tuple const& kTuple) const
    {
        return (x*x + y*y > kTuple.x*kTuple.x + kTuple.y*kTuple.y);
    }

    bool operator>=(Tuple const& kTuple) const
    {
        return (x*x + y*y >= kTuple.x*kTuple.x + kTuple.y*kTuple.y);
    }

    bool operator==(Tuple const& kTuple) const
    {
        return (x == kTuple.x && y == kTuple.y);
    }

    bool operator!=(Tuple const& kTuple) const
    {
        return (x != kTuple.x || y != kTuple.y);
    }


// automatic type conversion

    operator Tuple<uint>() const
    {
        return Tuple<uint>(static_cast<uint>(x), static_cast<uint>(y));
    }


    operator Tuple<int>(void) const
    {
        return Tuple<int>(static_cast<int>(x), static_cast<int>(y));
    }

    // public member variables
    T	x;
    T   y;
};

#endif
