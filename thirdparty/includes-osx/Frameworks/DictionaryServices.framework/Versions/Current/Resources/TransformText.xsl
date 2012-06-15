<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:d="http://www.apple.com/DTDs/DictionaryService-1.0.rng"
				version="1.0">
<xsl:output method="html" encoding="UTF-8" indent="no"
	doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
	doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" />

<!--
	Root level template
-->
<xsl:template match="/">
<text>
	<xsl:apply-templates select="//d:entry" />
</text>
</xsl:template>

<!--
	Replace gaiji representation
-->
<xsl:template match="//*[@d:alt]">
	<xsl:apply-templates select="@d:alt" />
</xsl:template>

</xsl:stylesheet>
