/////////////////////////////////////////////////
// String-Utilities
class TStrUtil {
public:
	static TStr GetStr(const TIntV& IntV, const TStr& DelimiterStr = ",");	
	static TStr GetStr(const TStrIntPrV& StrIntPrV, const TStr& FieldDelimiterStr = ":",
		const TStr& DelimiterStr = ",");	
	static TStr GetStr(const TFltV& FltV, const TStr& DelimiterStr = ",", const TStr& FmtStr = "%g");	
	static TStr GetStr(const TIntFltKdV& IntFltKdV, const TStr& FieldDelimiterStr = ":", 
		const TStr& DelimiterStr = ",", const TStr& FmtStr = "%g");	
	static TStr GetStr(const TStrV& StrV, const TStr& DelimiterStr = ",");	
	static TStr GetStr(const TStrH& StrH, const TStr& FieldDelimiterStr = ":", const TStr& DelimiterStr = ",");	
};
