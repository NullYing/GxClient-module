#include "stdafx.h"
#include "BaseFuntion.h"
#include "png.h"
#include "time.h"
#include "ZTECheckCode.h"

#define PNG_BYTES_TO_CHECK 4
ACODE img;
ACODE buf[4];

int load_png_image(FILE *fp, ACODE *img)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;
	char buf[PNG_BYTES_TO_CHECK];
	int w, h, x, y, temp, color_type, block_size;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	info_ptr = png_create_info_struct(png_ptr);

	setjmp(png_jmpbuf(png_ptr));
	/* ��ȡPNG_BYTES_TO_CHECK���ֽڵ����� */
	temp = fread(buf, 1, PNG_BYTES_TO_CHECK, fp);
	/* �����������ݲ�û��PNG_BYTES_TO_CHECK���ֽ� */
	if (temp < PNG_BYTES_TO_CHECK) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return 0;
	}
	/* ��������Ƿ�ΪPNG��ǩ�� */
	temp = png_sig_cmp((png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK);
	/* �������PNG��ǩ������˵�����ļ�����PNG�ļ� */
	if (temp != 0) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return 0;
	}

	/* ��λ�ļ�ָ�� */
	rewind(fp);
	/* ��ʼ���ļ� */
	png_init_io(png_ptr, fp);
	/* ��ȡPNGͼƬ��Ϣ */
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	/* ��ȡͼ���ɫ������ */
	color_type = png_get_color_type(png_ptr, info_ptr);
	/* ��ȡͼ��Ŀ�� */
	w = png_get_image_width(png_ptr, info_ptr);
	h = png_get_image_height(png_ptr, info_ptr);
	img->h = h;
	img->w = w;
	/* ��ȡͼ����������������ݣ�row_pointers��߾���rgba���� */
	row_pointers = png_get_rows(png_ptr, info_ptr);
	/* ���ݲ�ͬ��ɫ�����ͽ�����Ӧ���� */

	block_size = color_type == 6 ? 4 : 3;

	for (x = 0; x < h; x++) {
		for (y = 0; y<w * block_size; y += block_size) {
			BYTE  r, g, b, a;
			/* ������RGBA���ݣ���Ҫ���Լ�������룬����RGBA���� */
			r = row_pointers[x][y + 0]; // red
			g = row_pointers[x][y + 1]; // green
			b = row_pointers[x][y + 2]; // blue
			a = row_pointers[x][y + 3]; // alpha
			img->word[x][y / block_size] = (r + g + b) > 382 ? 0 : 1;
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	return 0;
}

void cut(ACODE *buf, ACODE *img) {
	int x1, y1, x2, y2, i, j, k, l, n, m;

	ACODE tmp;

	for (x1 = 0, i = 0, j = 0, k = 0, l = 0; x1 < img->w; x1++, l = i, i = 0) {
		for (y1 = 0; y1 < img->h; y1++) {
			i += img->word[y1][x1];
			if (i != 0) tmp.word[y1][j] = img->word[y1][x1];
		}
		if (i != 0) j++;
		if (i == 0 && l != 0) {
			for (y2 = 0, n = 0, m = 0; y2 < img->h; y2++, m = i, i = 0) {
				for (x2 = 0; x2 < j; x2++) {
					i += tmp.word[y2][x2];
					if (i != 0) buf[k].word[n][x2] = tmp.word[y2][x2];
				}
				if (i != 0) n++;
				if (i == 0 && m != 0) {
					buf[k].h = n;
					buf[k].w = j;
					break;
				}
			}
			memset(&tmp.word, 0, sizeof(tmp.word));
			k++,
				j = 0;
		}
	}
}

char pick(ACODE *buf) {
	int x, y, i;
	float max;
	char maxc;
	max = sample[0].score;;
	maxc = sample[0].dst;
	for (i = 0; i < 16; i++) {
		sample[i].score = 0;
		for (y = 0; y < sample[i].h; y++) {
			for (x = 0; x < sample[i].w; x++) {
				if ((sample[i].src[y * sample[i].w + x] - 48) == buf->word[y][x]) sample[i].score++;
			}
		}
		sample[i].score = sample[i].score / (sample[i].h * sample[i].w);
		if (max <= sample[i].score) max = sample[i].score, maxc = sample[i].dst;
	}
	return maxc;
}
/*
int display(FILE *fo, ACODE *buf) {
	int x, y;

	for (y = 0; y < buf->h; y++) {
		for (x = 0; x < buf->w; x++) {
			if (buf->word[y][x]>0) {
				fprintf(fo, "*");
			}
			else {
				fprintf(fo, " ");
			}
		}
		fprintf(fo, "\n");
	}
}

int savedata(ACODE *buf, char a) {
	int x, y;
	char fbuf[100];
	sprintf(fbuf, "%c.txt", a);
	FILE *fp = fopen(fbuf, "wb");
	if (!fp) return 0;
	for (y = 0; y < buf->h; y++) {
		for (x = 0; x < buf->w; x++) {
			if (buf->word[y][x]>0) {
				fprintf(fp, "1");
			}
			else {
				fprintf(fp, "0");
			}
		}
	}
	fprintf(fp, "\n%d, %d", buf->w, buf->h);
	fclose(fp);
}
*/
BASEFUNTION_API int ZTECheckCode(const char *authcodepath, char *text)
{
	//double  start;
	// fbuf[100];
	FILE *fp;

	//start = clock();

	fp = fopen(authcodepath, "rb");
	if (!fp) return 0;
	load_png_image(fp, &img);
	fclose(fp);
	memset(&buf, 0, sizeof(buf));
	cut(buf, &img);
	for (int j = 0; j < 4; j++) {
		text[j] = pick(&buf[j]);
	}
	text[4] = '\0';
	//printf("%s : %s in %f seconds\n", fbuf, text, (clock() - start) / CLOCKS_PER_SEC);
	//printf("total %f seconds\n", (clock() - start) / CLOCKS_PER_SEC);

	return 1;
}