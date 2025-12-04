#include <gfx/print.h>
#include <gfx/draw.h>


extern unsigned char _binary_build2_font_raw_start[];
extern unsigned char _binary_build2_font_raw_end[];

print_pos_data_t print_pos_data = {0};

flocal uint8_t*
get_glyph(char c)
{
    return _binary_build2_font_raw_start + (c * FONT_HEIGHT);
}

static void
out(char c)
{
    int x = print_pos_data.x;
    int y = print_pos_data.y;

    if (c == '\n') 
    {
        print_pos_data.x = 0;
        print_pos_data.y += FONT_HEIGHT;
        return;
    }

    uint8_t* glyph = get_glyph(c);

    for (int row = 0; row < FONT_HEIGHT; row++)
    {
        uint8_t bits = glyph[row];

        for (int col = 0; col < FONT_WIDTH; col++)
        {
            if (bits & (0x80 >> col)) {
                draw_pixel(x + col, y + row, 0xFFFFFFFF);
            }
        }
    }

    print_pos_data.x += FONT_WIDTH;
}

static void
print_char(char** buffer, char c)
{
    if (buffer)
        *(*buffer)++ = c;
    else
        out((uint8_t)c);
}

static int
vsnprintf(char* buffer, size_t size, const char* format, va_list args)
{
    char* buf_ptr = buffer;
    const char* end = buffer ? buffer + size - 1 : NULL;

    while (*format)
    {
        if (*format != '%')
        {
            if (!end || buf_ptr < end)
                print_char(buffer ? &buf_ptr : NULL, *format);
            format++;
            continue;
        }

        format++; // skip '%'
        if (*format == '\0') break;

        int long_flag = 0;
        int long_long_flag = 0;

        // Handle length modifiers: l, ll
        if (*format == 'l')
        {
            format++;
            if (*format == 'l')
            {
                long_long_flag = 1;
                format++;
            }
            else
            {
                long_flag = 1;
            }
        }

        switch (*format)
        {
            case 'c':
            {
                char c = (char)va_arg(args, int);
                if (!end || buf_ptr < end)
                    print_char(buffer ? &buf_ptr : NULL, c);
                break;
            }
            case 's':
            {
                const char* s = va_arg(args, const char*);
                while (*s)
                {
                    if (!end || buf_ptr < end)
                        print_char(buffer ? &buf_ptr : NULL, *s);
                    s++;
                }
                break;
            }
            case 'd':
            case 'i':
            {
                long long num = 0;
                if (long_long_flag)
                    num = va_arg(args, long long);
                else if (long_flag)
                    num = va_arg(args, long);
                else
                    num = va_arg(args, int);

                if (num < 0)
                {
                    if (!end || buf_ptr < end)
                        print_char(buffer ? &buf_ptr : NULL, '-');
                    num = -num;
                }

                char tmp[21]; // Enough for 64-bit signed
                int i = 19;
                tmp[20] = '\0';
                if (num == 0) tmp[i--] = '0';
                while (num && i >= 0)
                {
                    tmp[i--] = '0' + (num % 10);
                    num /= 10;
                }
                for (int j = i + 1; tmp[j]; j++)
                    if (!end || buf_ptr < end)
                        print_char(buffer ? &buf_ptr : NULL, tmp[j]);
                break;
            }
            case 'u':
            {
                unsigned long long num = 0;
                if (long_long_flag)
                    num = va_arg(args, unsigned long long);
                else if (long_flag)
                    num = va_arg(args, unsigned long);
                else
                    num = va_arg(args, unsigned int);

                char tmp[21];
                int i = 19;
                tmp[20] = '\0';
                if (num == 0) tmp[i--] = '0';
                while (num && i >= 0)
                {
                    tmp[i--] = '0' + (num % 10);
                    num /= 10;
                }
                for (int j = i + 1; tmp[j]; j++)
                    if (!end || buf_ptr < end)
                        print_char(buffer ? &buf_ptr : NULL, tmp[j]);
                break;
            }
            case 'x':
            case 'X':
            {
                unsigned long long num = 0;
                if (long_long_flag)
                    num = va_arg(args, unsigned long long);
                else if (long_flag)
                    num = va_arg(args, unsigned long);
                else
                    num = va_arg(args, unsigned int);

                const char* hex_chars = (*format == 'x') ? "0123456789abcdef" : "0123456789ABCDEF";

                char tmp[17];
                int i = 15;
                tmp[16] = '\0';
                if (num == 0) tmp[i--] = '0';
                while (num && i >= 0)
                {
                    tmp[i--] = hex_chars[num & 0xF];
                    num >>= 4;
                }
                for (int j = i + 1; tmp[j]; j++)
                    if (!end || buf_ptr < end)
                        print_char(buffer ? &buf_ptr : NULL, tmp[j]);
                break;
            }
            case 'f':
            {
                double d = va_arg(args, double);
                if (d < 0)
                {
                    if (!end || buf_ptr < end)
                        print_char(buffer ? &buf_ptr : NULL, '-');
                    d = -d;
                }

                long long int_part = (long long)d;
                double frac = d - (double)int_part;

                // Print integer part
                char tmp[21];
                int i = 19;
                tmp[20] = '\0';
                if (int_part == 0) tmp[i--] = '0';
                while (int_part && i >= 0)
                {
                    tmp[i--] = '0' + (int_part % 10);
                    int_part /= 10;
                }
                for (int j = i + 1; tmp[j]; j++)
                    if (!end || buf_ptr < end)
                        print_char(buffer ? &buf_ptr : NULL, tmp[j]);

                // Print decimal point
                if (!end || buf_ptr < end)
                    print_char(buffer ? &buf_ptr : NULL, '.');

                // Print 6 digits after decimal (fixed)
                for (int k = 0; k < 6; ++k)
                {
                    frac *= 10.0;
                    int digit = (int)frac;
                    if (!end || buf_ptr < end)
                        print_char(buffer ? &buf_ptr : NULL, '0' + digit);
                    frac -= digit;
                }
                break;
            }
            case '%':
                if (!end || buf_ptr < end)
                    print_char(buffer ? &buf_ptr : NULL, '%');
                break;

            default:
                // Unknown specifier, ignore
                break;
        }
        format++;
    }

    if (buffer)
        *buf_ptr = '\0';

    return buf_ptr - buffer;
}

static int
snprintf(char* buffer, size_t size, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, size, format, args);
    va_end(args);
    return len;
}

int
kprintf(const char* format, ...)
{
    char buffer[PRINTF_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    for (int i = 0; i < len; ++i)
    {
        out(buffer[i]);
    }

    return len;
}