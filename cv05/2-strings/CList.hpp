#pragma once

#include "SStudent.hpp"

class CList
{
	public:
        CList ( void );
        ~CList ( void );

        CList              ( const CList & ) = delete;
        CList & operator = ( const CList & ) = delete;
        CList              ( CList && )      = delete;
        CList & operator = ( CList && )      = delete;

		CList & add_student ( const SStudent & student );
		const SStudent & get_student ( size_t idx ) const;
		size_t size ( void ) const;

        using const_iterator = const SStudent *;
        using iterator = SStudent *;

        const_iterator begin ( void ) const;
        const_iterator end ( void ) const;
        iterator begin ( void );
        iterator end ( void );
	private:
		size_t m_used;
        size_t m_capacity;
        SStudent * m_data;
        void expand ( void );
};
