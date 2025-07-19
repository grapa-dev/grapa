// GrapaBtreeBlock.cpp
/*
Copyright 2022 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#include "GrapaMem.h"
#include "GrapaBtreeBlock.h"

#include <string.h>

void GrapaBlockNodeHeader::BigEndian()
{
	weight = BE_S64(weight);
	parent = BE_S64(parent);
	firstChild = BE_S64(firstChild);
}
void GrapaBlockNodeHeader::Init() { memset(&blockType, 0, GetSize()); blockType = GrapaBlock::NODE_BLOCK; }
GrapaError GrapaBlockNodeHeader::Write(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	BigEndian();
	GrapaError err = pFile->Write(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	return(err);
}
GrapaError GrapaBlockNodeHeader::Read(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	GrapaError err = pFile->Read(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	if (err) Init();
	return(err);
}

void GrapaBlockNodeLeaf::BigEndian()
{
	key = BE_S64(key);
	value = BE_S64(value);
	child = BE_S64(child);
}
u64 GrapaBlockNodeLeaf::GetOffset(u64 blockPos, s8 indexPos) { return(blockPos + GrapaBlockNodeHeader::GetBlocks() + (((u64)indexPos)*GetBlocks())); }
void GrapaBlockNodeLeaf::Init() { memset(&blockType, 0, GetSize()); blockType = GrapaBlock::LEAF_BLOCK; }
GrapaError GrapaBlockNodeLeaf::Write(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	BigEndian();
	GrapaError err = pFile->Write(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	return(err);
}
GrapaError GrapaBlockNodeLeaf::Write(GrapaFile *pFile, u64 blockPos, s8 indexPos)
{
	return Write(pFile, GetOffset(blockPos, indexPos));
}
GrapaError GrapaBlockNodeLeaf::Read(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	GrapaError err = pFile->Read(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	if (err) Init();
	return(err);
}
GrapaError GrapaBlockNodeLeaf::Read(GrapaFile *pFile, u64 blockPos, s8 indexPos)
{
	return Read(pFile, GetOffset(blockPos, indexPos));
}

void GrapaBlockDataHeader::BigEndian()
{
	parentTree = BE_S64(parentTree);
	growBlockSize = BE_S64(growBlockSize);
	growSize = BE_S64(growSize);
	dataSize = BE_S64(dataSize);
	dataLength = BE_S64(dataLength);
	dataStart = BE_S64(dataStart);
}
void GrapaBlockDataHeader::Init() { memset(&blockType, 0, GetSize()); blockType = GrapaBlock::DATA_BLOCK; }
GrapaError GrapaBlockDataHeader::Write(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	BigEndian();
	GrapaError err = pFile->Write(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	return(err);
}
GrapaError GrapaBlockDataHeader::Read(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	GrapaError err = pFile->Read(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	if (err) Init();
	return(err);
}

void GrapaBlockPage::BigEndian()
{
	blockCount = BE_S64(blockCount);
	nextPage = BE_S64(nextPage);
}
void GrapaBlockPage::Init() { memset(&blockType, 0, GetSize()); blockType = GrapaBlock::PAGE_BLOCK; }
GrapaError GrapaBlockPage::Write(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	BigEndian();
	GrapaError err = pFile->Write(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	return(err);
}
GrapaError GrapaBlockPage::Read(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	GrapaError err = pFile->Read(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	if (err) Init();
	return(err);
}

void GrapaBlockTree::BigEndian()
{
	itemCount = BE_S64(itemCount);
	firstItem = BE_S64(firstItem);
	storeTree = BE_S64(storeTree);
	indexTree = BE_S64(indexTree);
	parentTree = BE_S64(parentTree);
}
void GrapaBlockTree::Init() { memset(&blockType, 0, GetSize()); blockType = GrapaBlock::TREE_BLOCK; }
bool GrapaBlockTree::GetDirty() { return((flags & 0x80) == 0x80); }
void GrapaBlockTree::SetDirty(bool isDirty) { if (isDirty) flags |= 0x80; else flags &= 0x7F; }
GrapaError GrapaBlockTree::Write(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	BigEndian();
	GrapaError err = pFile->Write(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	return(err);
}
GrapaError GrapaBlockTree::Read(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	GrapaError err = pFile->Read(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	if (err) Init();
	return(err);
}

void GrapaBlockFileHeader::BigEndian()
{
	firstTree = BE_S64(firstTree);
	firstBlock = BE_S64(firstBlock);
}
void GrapaBlockFileHeader::Init() { memset(&blockType, 0, GetSize()); blockType = GrapaBlock::FILE_BLOCK; fileRef1 = 'B'; fileRef2 = 'T'; blockSize = GrapaBlock::BLOCKSIZE; }
GrapaError GrapaBlockFileHeader::Write(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	BigEndian();
	GrapaError err = pFile->Write(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	return(err);
}
GrapaError GrapaBlockFileHeader::Read(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	GrapaError err = pFile->Read(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	if (err) Init();
	return(err);
}

void GrapaBlockFirst::BigEndian()
{
	blockCount = BE_S64(blockCount);
	firstPage = BE_S64(firstPage);
}
void GrapaBlockFirst::Init() { memset(&blockType, 0, GetSize()); blockType = GrapaBlock::FIRST_BLOCK; }
GrapaError GrapaBlockFirst::Write(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	BigEndian();
	GrapaError err = pFile->Write(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	return(err);
}
GrapaError GrapaBlockFirst::Read(GrapaFile *pFile, u64 blockPos)
{
	if (!pFile) return(-1);
	GrapaError err = pFile->Read(blockPos, GrapaBlock::BLOCKSIZE, 0, GetSize(), &blockType);
	BigEndian();
	if (err) Init();
	return(err);
}

////////////////////////////////////////////////////////////////////////////////
//	20-Jun-01	cmatichuk	Created
