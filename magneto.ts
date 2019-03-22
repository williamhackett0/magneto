/**
 * Provides access to all the banana functionality.
 */
// Icon unicode characters can be found at: http://fontawesome.io/icons/
//% color=190 weight=100 icon="\uf1ec" block="Magneto Blocks" advanced=false
namespace magneto {
    /**
     * Test function for Magneto
     */
    //% weight=50
    //% blockId=magneto_get_number block="Gets a number from magneto" blockGap=8
    //% shim=magneto::testFunction
    export function testFunction(): number;
    
    /**
     * Test function for Magneto
     */
    //% weight=50
    //% blockId=magneto_test block="Dunno" blockGap=8
    export function hmmm(): number{
        return 2;
    }
}
