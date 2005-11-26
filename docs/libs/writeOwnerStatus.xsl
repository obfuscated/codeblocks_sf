<?xml version="1.0" encoding="UTF-8"?>
<!--
   This stylesheet gathers information from the authorblurbs embedded in the documents and generates a Docbook Article.
   
   Author: Sean Wheller sean@inwords.co.za http://www.inwords.co.za
 -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    <xsl:output doctype-public="-//OASIS//DTD DocBook XML V4.3//EN"
        doctype-system="http://www.oasis-open.org/docbook/xml/4.3/docbookx.dtd" encoding="UTF-8"
        indent="yes" version="1.0"/>
    <xsl:template match="/">
        <article>
            <title>
                <xsl:value-of select="//title/."/>
            </title>
            <subtitle>Documentation Project Status Report</subtitle>
            <informaltable>
                <tgroup cols="2">
                    <thead>
                        <row>
                            <entry>TOC Entry</entry>
                            <entry>Author Remarks</entry>
                        </row>
                    </thead>
                    <tbody>
                        <xsl:call-template name="doItems"/>
                    </tbody>
                </tgroup>
            </informaltable>
        </article>
    </xsl:template>
    <xsl:template name="doItems">
        <xsl:for-each select="//chapter|//sect1|//sect2|//sect3|//sect4|//sect5">
            <row>
                <entry>
                    <xsl:value-of select="title"/>
                </entry>
                <entry>
                    <xsl:choose>
                        <xsl:when test="@status='todo'">
                            <para>
                                <emphasis role="bold">
                                    <xsl:text>Has to be done</xsl:text>
                                </emphasis>
                            </para>
                            <xsl:call-template name="doAuthor"/>
                        </xsl:when>
                        <xsl:when test="@status='help'">
                            <para>
                                <emphasis role="bold">
                                    <xsl:text>Help Wanted</xsl:text>
                                </emphasis>
                            </para>
                            <xsl:call-template name="doAuthor"/>
                        </xsl:when>
                        <xsl:when test="@status='writing'">
                            <para>
                                <xsl:text>In Progress</xsl:text>
                                <xsl:call-template name="doAuthor"/>
                            </para>
                        </xsl:when>
                        <xsl:when test="@status='review'">
                            <para>
                                <emphasis>
                                    <xsl:text>Awaiting Review</xsl:text>
                                </emphasis>
                            </para>
                            <xsl:call-template name="doAuthor"/>
                        </xsl:when>
                        <xsl:when test="@status='reviewing'">
                            <para>
                                <emphasis>
                                    <xsl:text>In Review</xsl:text>
                                </emphasis>
                            </para>
                            <xsl:call-template name="doAuthor"/>
                        </xsl:when>
                        <xsl:when test="@status='complete'">
                            <para>
                                <xsl:text>Finished</xsl:text>
                            </para>
                            <xsl:call-template name="doAuthor"/>
                        </xsl:when>
                        <xsl:otherwise>
                            <para>
                                <xsl:text>NO STATUS</xsl:text>
                            </para>
                        </xsl:otherwise>
                    </xsl:choose>
                </entry>
            </row>
        </xsl:for-each>
    </xsl:template>
    <xsl:template name="doAuthor">
        <xsl:if test="authorblurb">
            <para>
                <xsl:value-of select="authorblurb/para"/>
            </para>
        </xsl:if>
    </xsl:template>
</xsl:stylesheet>
