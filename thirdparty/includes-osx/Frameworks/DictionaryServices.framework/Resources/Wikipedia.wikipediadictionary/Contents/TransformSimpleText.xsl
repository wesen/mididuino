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
	<xsl:apply-templates select="//body" />
</text>
</xsl:template>

<xsl:template match="h1">
<xsl:value-of select="."/><xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="h2">
<xsl:apply-templates select="@*|node()" />
<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="p">
<xsl:apply-templates select="@*|node()" />
<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="li">
 - <xsl:apply-templates select="@*|node()" /><xsl:text> 
</xsl:text>
</xsl:template>

<!-- snipped elements -->
<xsl:template match="*[@class='editsection']" />
<xsl:template match="*[@class='printonly']" />
<xsl:template match="*[@class='noprint']" />
<xsl:template match="*[@class='hiddenStructure']" />
<xsl:template match="*[@class='metadata']" />
<xsl:template match="*[@id='coordinates']" />
<xsl:template match="*[@id='donate']" />
<xsl:template match="sup" />

</xsl:stylesheet>
