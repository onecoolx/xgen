typedef struct {
  unsigned num_words;
  unsigned word_ofs;
  const unsigned *data;
} section_t;

void test(section_t* restrict section, unsigned* restrict dst)
{
  while (section->data != 0) {
    const unsigned *src = section->data;
    for (unsigned i=0; i < section->num_words; ++i) {
    /*
    Author thinks "There is no need to reload section->word_ofs on every iteration of the
    for loop, but due to the limitation described above, we're unable to
    tell that section doesn't alias with dst inside the loop."

    But, I think section may alias with src. section should not alias with dst!
    */
      dst[section->word_ofs + i] = src[i];
    }

    ++section;
  }
}

