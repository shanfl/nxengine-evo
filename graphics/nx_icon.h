/* GIMP RGBA C-Source image dump (MyChar.c) */

static const struct
{
    unsigned int 	 width;
    unsigned int 	 height;
    unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    unsigned char	 pixel_data[16 * 16 * 4 + 1];
} WINDOW_TITLE_ICON =
{
    16, 16, 4,
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\346\357\332\377\346\357\332\377\346\357\332\377\346\357"
    "\332\377\346\357\332\377\346\357\332\377\346\357\332\377\377F)\377\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377F)\377\377F)\377\377F)\377\377F)\377"
    "\377F)\377\377F)\377\377F)\377\377F)\377\377F)\377\377F)\377\377F)\377\377"
    "F)\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377F)\377\214\245"
    "\275\377\214\245\275\377\214\245\275\377\214\245\275\377\0\20\0\377\0\20"
    "\0\377\0\20\0\377\0\20\0\377\377F)\377\0\0\0\0\234\377\37\377\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\214\245\275\377\10%w\377\214\245\275\377\346\357\332"
    "\377\346\357\332\377\346\357\332\377\346\357\332\377\10%w\377\214\245\275"
    "\377\0\20\0\377\0\0\0\0k\312g\377\0\0\0\0\0\0\0\0\0\0\0\0k\312g\377\214\245"
    "\275\377\10%w\377\346\357\332\377\346\357\332\377\346\357\332\377\346\357"
    "\332\377\346\357\332\377\10%w\377\346\357\332\377\0\20\0\377k\312g\377\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0k\312g\377\346\357\332\377\10%w\377\346\357"
    "\332\377\346\357\332\377\346\357\332\377\346\357\332\377\346\357\332\377"
    "\10%w\377\346\357\332\377\0\20\0\377k\312g\377\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\20\0\377\346\357\332\377\346\357\332\377\346\357\332\377"
    "\346\357\332\377\346\357\332\377\346\357\332\377\346\357\332\377\346\357"
    "\332\377\0\20\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\20\0\377\0\20\0\377\0\20\0\377\214\245\275\377\0\20\0\377\0\20\0\377"
    "\0\20\0\377\0\20\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\214\245\275\377\0\20\0\377\346\357\332\377"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\20\0\377\0\20\0\377\0\20\0\377\346\357"
    "\332\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\20\0\377\0\20\0\377\0\20\0\377\346\357"
    "\332\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\346\357\332\377\377F)\377\377F)\377\346\357\332"
    "\377\346\357\332\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377F)\377\377F)\377\377F)\377"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377F)\377\377F)\377\377F)\377\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\20\0\377\0\20\0\377\0\20\0\377\0\20\0\377\0\20\0\377\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
};

