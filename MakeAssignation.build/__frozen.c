// This provides the frozen (compiled bytecode) files that are included if
// any.
#include <Python.h>

#include "nuitka/constants_blob.h"

// Blob from which modules are unstreamed.
#define stream_data constant_bin

// These modules should be loaded as bytecode. They may e.g. have to be loadable
// during "Py_Initialize" already, or for irrelevance, they are only included
// in this un-optimized form. These are not compiled by Nuitka, and therefore
// are not accelerated at all, merely bundled with the binary or module, so
// that CPython library can start out finding them.

struct frozen_desc {
    char const *name;
    ssize_t start;
    int size;
};

void copyFrozenModulesTo( struct _frozen *destination )
{
    struct frozen_desc frozen_modules[] = {
        { "base64", 6369665, 11253 },
        { "codecs", 6380918, 36628 },
        { "copy_reg", 6417546, 5137 },
        { "encodings", 6422683, -4362 },
        { "encodings.aliases", 6427045, 8760 },
        { "encodings.ascii", 6435805, 2253 },
        { "encodings.base64_codec", 6438058, 3829 },
        { "encodings.big5", 6441887, 1748 },
        { "encodings.big5hkscs", 6443635, 1788 },
        { "encodings.bz2_codec", 6445423, 4721 },
        { "encodings.charmap", 6450144, 3465 },
        { "encodings.cp037", 6453609, 2829 },
        { "encodings.cp1006", 6456438, 2915 },
        { "encodings.cp1026", 6459353, 2843 },
        { "encodings.cp1140", 6462196, 2829 },
        { "encodings.cp1250", 6465025, 2866 },
        { "encodings.cp1251", 6467891, 2863 },
        { "encodings.cp1252", 6470754, 2866 },
        { "encodings.cp1253", 6473620, 2879 },
        { "encodings.cp1254", 6476499, 2868 },
        { "encodings.cp1255", 6479367, 2887 },
        { "encodings.cp1256", 6482254, 2865 },
        { "encodings.cp1257", 6485119, 2873 },
        { "encodings.cp1258", 6487992, 2871 },
        { "encodings.cp424", 6490863, 2859 },
        { "encodings.cp437", 6493722, 8064 },
        { "encodings.cp500", 6501786, 2829 },
        { "encodings.cp720", 6504615, 2926 },
        { "encodings.cp737", 6507541, 8292 },
        { "encodings.cp775", 6515833, 8078 },
        { "encodings.cp850", 6523911, 7811 },
        { "encodings.cp852", 6531722, 8080 },
        { "encodings.cp855", 6539802, 8261 },
        { "encodings.cp856", 6548063, 2891 },
        { "encodings.cp857", 6550954, 7801 },
        { "encodings.cp858", 6558755, 7781 },
        { "encodings.cp860", 6566536, 8047 },
        { "encodings.cp861", 6574583, 8058 },
        { "encodings.cp862", 6582641, 8193 },
        { "encodings.cp863", 6590834, 8058 },
        { "encodings.cp864", 6598892, 8189 },
        { "encodings.cp865", 6607081, 8058 },
        { "encodings.cp866", 6615139, 8293 },
        { "encodings.cp869", 6623432, 8105 },
        { "encodings.cp874", 6631537, 2957 },
        { "encodings.cp875", 6634494, 2826 },
        { "encodings.cp932", 6637320, 1756 },
        { "encodings.cp949", 6639076, 1756 },
        { "encodings.cp950", 6640832, 1756 },
        { "encodings.euc_jis_2004", 6642588, 1812 },
        { "encodings.euc_jisx0213", 6644400, 1812 },
        { "encodings.euc_jp", 6646212, 1764 },
        { "encodings.euc_kr", 6647976, 1764 },
        { "encodings.gb18030", 6649740, 1772 },
        { "encodings.gb2312", 6651512, 1764 },
        { "encodings.gbk", 6653276, 1740 },
        { "encodings.hex_codec", 6655016, 3781 },
        { "encodings.hp_roman8", 6658797, 4112 },
        { "encodings.hz", 6662909, 1732 },
        { "encodings.idna", 6664641, 6368 },
        { "encodings.iso2022_jp", 6671009, 1801 },
        { "encodings.iso2022_jp_1", 6672810, 1817 },
        { "encodings.iso2022_jp_2", 6674627, 1817 },
        { "encodings.iso2022_jp_2004", 6676444, 1841 },
        { "encodings.iso2022_jp_3", 6678285, 1817 },
        { "encodings.iso2022_jp_ext", 6680102, 1833 },
        { "encodings.iso2022_kr", 6681935, 1801 },
        { "encodings.iso8859_1", 6683736, 2868 },
        { "encodings.iso8859_10", 6686604, 2883 },
        { "encodings.iso8859_11", 6689487, 2977 },
        { "encodings.iso8859_13", 6692464, 2886 },
        { "encodings.iso8859_14", 6695350, 2904 },
        { "encodings.iso8859_15", 6698254, 2883 },
        { "encodings.iso8859_16", 6701137, 2885 },
        { "encodings.iso8859_2", 6704022, 2868 },
        { "encodings.iso8859_3", 6706890, 2875 },
        { "encodings.iso8859_4", 6709765, 2868 },
        { "encodings.iso8859_5", 6712633, 2869 },
        { "encodings.iso8859_6", 6715502, 2913 },
        { "encodings.iso8859_7", 6718415, 2876 },
        { "encodings.iso8859_8", 6721291, 2907 },
        { "encodings.iso8859_9", 6724198, 2868 },
        { "encodings.johab", 6727066, 1756 },
        { "encodings.koi8_r", 6728822, 2890 },
        { "encodings.koi8_u", 6731712, 2876 },
        { "encodings.latin_1", 6734588, 2283 },
        { "encodings.mac_arabic", 6736871, 8014 },
        { "encodings.mac_centeuro", 6744885, 2937 },
        { "encodings.mac_croatian", 6747822, 2945 },
        { "encodings.mac_cyrillic", 6750767, 2935 },
        { "encodings.mac_farsi", 6753702, 2849 },
        { "encodings.mac_greek", 6756551, 2889 },
        { "encodings.mac_iceland", 6759440, 2928 },
        { "encodings.mac_latin2", 6762368, 4907 },
        { "encodings.mac_roman", 6767275, 2906 },
        { "encodings.mac_romanian", 6770181, 2946 },
        { "encodings.mac_turkish", 6773127, 2929 },
        { "encodings.palmos", 6776056, 3066 },
        { "encodings.ptcp154", 6779122, 4890 },
        { "encodings.punycode", 6784012, 7942 },
        { "encodings.quopri_codec", 6791954, 3647 },
        { "encodings.raw_unicode_escape", 6795601, 2202 },
        { "encodings.rot_13", 6797803, 3656 },
        { "encodings.shift_jis", 6801459, 1788 },
        { "encodings.shift_jis_2004", 6803247, 1828 },
        { "encodings.shift_jisx0213", 6805075, 1828 },
        { "encodings.string_escape", 6806903, 2061 },
        { "encodings.tis_620", 6808964, 2938 },
        { "encodings.undefined", 6811902, 2589 },
        { "encodings.unicode_escape", 6814491, 2150 },
        { "encodings.unicode_internal", 6816641, 2176 },
        { "encodings.utf_16", 6818817, 5160 },
        { "encodings.utf_16_be", 6823977, 1990 },
        { "encodings.utf_16_le", 6825967, 1990 },
        { "encodings.utf_32", 6827957, 5724 },
        { "encodings.utf_32_be", 6833681, 1883 },
        { "encodings.utf_32_le", 6835564, 1883 },
        { "encodings.utf_7", 6837447, 1883 },
        { "encodings.utf_8", 6839330, 1942 },
        { "encodings.utf_8_sig", 6841272, 4977 },
        { "encodings.uu_codec", 6846249, 4909 },
        { "encodings.zlib_codec", 6851158, 4641 },
        { "functools", 6855799, 6561 },
        { "locale", 6862360, 56524 },
        { "quopri", 6918884, 6544 },
        { "re", 6925428, 13363 },
        { "sre_compile", 6938791, 12522 },
        { "sre_constants", 6951313, 6177 },
        { "sre_parse", 6957490, 21076 },
        { "string", 6978566, 20349 },
        { "stringprep", 6998915, 14439 },
        { "struct", 7013354, 229 },
        { "types", 7013583, 2703 },
        { NULL, 0, 0 }
    };

    struct frozen_desc *current = frozen_modules;

    for(;;)
    {
        destination->name = (char *)current->name;
        destination->code = (unsigned char *)&constant_bin[ current->start ];
        destination->size = current->size;

        if (destination->name == NULL) break;

        current += 1;
        destination += 1;
    };
}
