#ifndef MII_H_INCLUDED
#define MII_H_INCLUDED

/*! \name Generic MII registers.
 */
//! @{
#define PHY_BMCR            0x00        //!< Basic mode control register
#define PHY_BMSR            0x01        //!< Basic mode status register
#define PHY_PHYSID1         0x02        //!< PHYS ID 1
#define PHY_PHYSID2         0x03        //!< PHYS ID 2
#define PHY_ADVERTISE       0x04        //!< Advertisement control reg
#define PHY_LPA             0x05        //!< Link partner ability reg
//! @}


/*! \name Basic mode control register.
 */
//! @{
#define BMCR_RESV               0x007f  //!< Unused...
#define BMCR_CTST               0x0080  //!< Collision test
#define BMCR_FULLDPLX           0x0100  //!< Full duplex
#define BMCR_ANRESTART          0x0200  //!< Auto negotiation restart
#define BMCR_ISOLATE            0x0400  //!< Disconnect PHY from MII
#define BMCR_PDOWN              0x0800  //!< Powerdown the PHY
#define BMCR_ANENABLE           0x1000  //!< Enable auto negotiation
#define BMCR_SPEED100           0x2000  //!< Select 100Mbps
#define BMCR_LOOPBACK           0x4000  //!< TXD loopback bits
#define BMCR_RESET              0x8000  //!< Reset the PHY
//! @}

/*! \name Basic mode status register.
 */
//! @{
#define BMSR_ERCAP              0x0001  //!< Ext-reg capability
#define BMSR_JCD                0x0002  //!< Jabber detected
#define BMSR_LSTATUS            0x0004  //!< Link status
#define BMSR_ANEGCAPABLE        0x0008  //!< Able to do auto-negotiation
#define BMSR_RFAULT             0x0010  //!< Remote fault detected
#define BMSR_ANEGCOMPLETE       0x0020  //!< Auto-negotiation complete
#define BMSR_RESV               0x00c0  //!< Unused...
#define BMSR_ESTATEN            0x0100  //!< Extended Status in R15
#define BMSR_100FULL2           0x0200  //!< Can do 100BASE-T2 HDX
#define BMSR_100HALF2           0x0400  //!< Can do 100BASE-T2 FDX
#define BMSR_10HALF             0x0800  //!< Can do 10mbps, half-duplex
#define BMSR_10FULL             0x1000  //!< Can do 10mbps, full-duplex
#define BMSR_100HALF            0x2000  //!< Can do 100mbps, half-duplex
#define BMSR_100FULL            0x4000  //!< Can do 100mbps, full-duplex
#define BMSR_100BASE4           0x8000  //!< Can do 100mbps, 4k packets
//! @}

/*! \name Advertisement control register.
 */
//! @{
#define ADVERTISE_SLCT          0x001f  //!< Selector bits
#define ADVERTISE_CSMA          0x0001  //!< Only selector supported
#define ADVERTISE_10HALF        0x0020  //!< Try for 10mbps half-duplex
#define ADVERTISE_1000XFULL     0x0020  //!< Try for 1000BASE-X full-duplex
#define ADVERTISE_10FULL        0x0040  //!< Try for 10mbps full-duplex
#define ADVERTISE_1000XHALF     0x0040  //!< Try for 1000BASE-X half-duplex
#define ADVERTISE_100HALF       0x0080  //!< Try for 100mbps half-duplex
#define ADVERTISE_1000XPAUSE    0x0080  //!< Try for 1000BASE-X pause
#define ADVERTISE_100FULL       0x0100  //!< Try for 100mbps full-duplex
#define ADVERTISE_1000XPSE_ASYM 0x0100  //!< Try for 1000BASE-X asym pause
#define ADVERTISE_100BASE4      0x0200  //!< Try for 100mbps 4k packets
#define ADVERTISE_PAUSE_CAP     0x0400  //!< Try for pause
#define ADVERTISE_PAUSE_ASYM    0x0800  //!< Try for asymmetric pause
#define ADVERTISE_RESV          0x1000  //!< Unused...
#define ADVERTISE_RFAULT        0x2000  //!< Say we can detect faults
#define ADVERTISE_LPACK         0x4000  //!< Ack link partners response
#define ADVERTISE_NPAGE         0x8000  //!< Next page bit
//! @}

#define ADVERTISE_FULL (ADVERTISE_100FULL | ADVERTISE_10FULL | ADVERTISE_CSMA)
#define ADVERTISE_ALL (ADVERTISE_10HALF | ADVERTISE_10FULL | \
                       ADVERTISE_100HALF | ADVERTISE_100FULL)

/*! \name Link partner ability register.
 */
//! @{
#define LPA_SLCT                0x001f  //!< Same as advertise selector
#define LPA_10HALF              0x0020  //!< Can do 10mbps half-duplex
#define LPA_1000XFULL           0x0020  //!< Can do 1000BASE-X full-duplex
#define LPA_10FULL              0x0040  //!< Can do 10mbps full-duplex
#define LPA_1000XHALF           0x0040  //!< Can do 1000BASE-X half-duplex
#define LPA_100HALF             0x0080  //!< Can do 100mbps half-duplex
#define LPA_1000XPAUSE          0x0080  //!< Can do 1000BASE-X pause
#define LPA_100FULL             0x0100  //!< Can do 100mbps full-duplex
#define LPA_1000XPAUSE_ASYM     0x0100  //!< Can do 1000BASE-X pause asym
#define LPA_100BASE4            0x0200  //!< Can do 100mbps 4k packets
#define LPA_PAUSE_CAP           0x0400  //!< Can pause
#define LPA_PAUSE_ASYM          0x0800  //!< Can pause asymmetrically
#define LPA_RESV                0x1000  //!< Unused...
#define LPA_RFAULT              0x2000  //!< Link partner faulted
#define LPA_LPACK               0x4000  //!< Link partner acked us
#define LPA_NPAGE               0x8000  //!< Next page bit

#define LPA_DUPLEX    (LPA_10FULL | LPA_100FULL)
#define LPA_100       (LPA_100FULL | LPA_100HALF | LPA_100BASE4)
//! @}

#ifndef EXTPHY_PHY_ADDR
/*! Phy Address (set through strap options) 
    This Define must be place in the board configuration
*/
#define EXTPHY_PHY_ADDR     0x01
#warning Set a default value : #define EXTPHY_PHY_ADDR 0x01
#endif 

/*! Phy Identifier for the DP83848 */
#define EXTPHY_PHY_ID       0x20005C90


/*! \name Extended registers for DP83848
 */
//! @{
#define PHY_STS				0x10	//!< PHY Status Register
#define PHY_RBR             0x17    //!< RMII Bypass reg
#define PHY_MICR            0x11    //!< Interrupt Control reg
#define PHY_MISR            0x12    //!< Interrupt Status reg
#define PHY_PHYCR           0x19    //!< Phy CTRL reg
//! @}

/*! RMII Bypass Register */
#define RBR_RMII            0x0020  //!< RMII Mode
/*! \name Interrupt Ctrl Register.
 */
//! @{
#define MICR_INTEN          0x0002  //!< Enable interrupts
#define MICR_INTOE          0x0001  //!< Enable INT output
//! @}

#define PHYSTS_SPEED_STATUS 0x0002	//!< Speed status
#define PHYSTS_DUPLEX_STATUS 0x0004	//!< Speed status



/*! \name Interrupt Status Register.
 */
//! @{
#define MISR_ED_INT_EN      0x0040  //!< Energy Detect enabled
#define MISR_LINK_INT_EN    0x0020  //!< Link status change enabled
#define MISR_SPD_INT_EN     0x0010  //!< Speed change enabled
#define MISR_DP_INT_EN      0x0008  //!< Duplex mode change enabled
#define MISR_ANC_INT_EN     0x0004  //!< Auto-Neg complete enabled
#define MISR_FHF_INT_EN     0x0002  //!< False Carrier enabled
#define MISR_RHF_INT_EN     0x0001  //!< Receive Error enabled
#define MISR_ED_INT         0x4000  //!< Energy Detect
#define MISR_LINK_INT       0x2000  //!< Link status change
#define MISR_SPD_INT        0x1000  //!< Speed change
#define MISR_DP_INT         0x0800  //!< Duplex mode change
#define MISR_ANC_INT        0x0400  //!< Auto-Neg complete
#define MISR_FHF_INT        0x0200  //!< False Carrier
#define MISR_RHF_INT        0x0100  //!< Receive Error
//! @}

/*! \name Phy Ctrl Register.
 */
//! @{
#define PHYCR_MDIX_EN       0x8000  //!< Enable Auto MDIX
#define PHYCR_MDIX_FORCE    0x4000  //!< Force MDIX crossed
//! @}

/**@}*/
#endif /* MII_H_INCLUDED */