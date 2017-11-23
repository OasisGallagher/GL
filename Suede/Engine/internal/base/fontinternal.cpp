#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include "variables.h"
#include "tools/path.h"
#include "tools/math2.h"
#include "tools/string.h"
#include "fontinternal.h"
#include "shaderinternal.h"
#include "textureinternal.h"
#include "materialinternal.h"
#include "internal/file/image.h"

FontInternal::FontInternal() 
	: ObjectInternal(ObjectTypeFont) ,size_(10), face_(nullptr), library_(nullptr) {
	material_ = CREATE_OBJECT(Material);
	Shader shader = CREATE_OBJECT(Shader);
	shader->Load("buildin/shaders/lit_texture");
	material_->SetShader(shader);
}

FontInternal::~FontInternal() {
	Destroy();
}

bool FontInternal::Load(const std::string& path, int size) {
	Destroy();
	return Import("resources/" + path, size);
}

bool FontInternal::Require(const std::string& str) {
	std::wstring wstr = String::MultiBytesToWideString(str);
	bool status = true;
	for (int i = 0; i < wstr.length(); ++i) {
		if (!glyphs_.contains(wstr[i])) {
			Bitmap* bitmap = &glyphs_[wstr[i]]->bitmap;
			bitmap->id = wstr[i];

			status = GetBitmapBits(wstr[i], bitmap) && status;
			bitmaps_.push_back(bitmap);
			material_->SetTexture(Variables::mainTexture, nullptr);
		}
	}

	return status;
}

Material FontInternal::GetMaterial() { 
	if (!material_->GetTexture(Variables::mainTexture)) {
		RebuildMaterial();
	}

	return material_; 
}

bool FontInternal::Import(const std::string& path, int size) {
	int err = 0;
	if ((err = FT_Init_FreeType(&library_)) != 0) {
		Debug::LogError(String::Format("failed to load font %s (%d)", path.c_str(), err));
		return false;
	}

	if ((err = FT_New_Face(library_, path.c_str(), 0, &face_)) != 0) {
		Debug::LogError(String::Format("failed to create font face for %s (%d)", path.c_str(), err));
		return false;
	}

	FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
	FT_Set_Char_Size(face_, size << 6, size << 6, 96, 96);
	
	fname_ = Path::GetFileName(path);
	size_ = size;

	return true;
}

bool FontInternal::GetBitmapBits(wchar_t wch, Bitmap* answer) {
	int err = 0;

	if ((err = FT_Load_Glyph(face_, FT_Get_Char_Index(face_, wch), FT_LOAD_DEFAULT) != 0)) {
		Debug::LogError(String::Format("failed to load glyph for char %d (%d)", wch, err));
		return false;
	}

	FT_Glyph glyph;
	if ((err = FT_Get_Glyph(face_->glyph, &glyph)) != 0) {
		Debug::LogError(String::Format("failed to get glyph for char %d (%d)", wch, err));
		return false;
	}

	if (answer == nullptr) {
		return true;
	}

	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
	const FT_Bitmap& bitmap = bitmapGlyph->bitmap;

	Bytes& data = answer->data;
	size_t size = bitmap.width * bitmap.rows;
	data.resize(size);
	std::copy(bitmap.buffer, bitmap.buffer + size, &data[0]);

	answer->width = bitmap.width;
	answer->height = bitmap.rows;
	answer->format = ColorFormatLuminanceAlpha;

	return true;
}

void FontInternal::RebuildMaterial() {
	AtlasMaker::Make(atlas_, bitmaps_, 2);

	Texture2D texture = CREATE_OBJECT(Texture2D);
	texture->Load(&atlas_.data[0], ColorFormatLuminanceAlpha, atlas_.width, atlas_.height);

	// release.
	atlas_.data.swap(Bytes());
	atlas_.width = atlas_.height = 0;

	material_->SetTexture(Variables::mainTexture, texture);
}

void FontInternal::Destroy() {
	if (face_ != nullptr) {
		FT_Done_Face(face_);
		face_ = nullptr;
	}

	if (library_ != nullptr) {
		FT_Done_FreeType(library_);
	}

	fname_.clear();
}
