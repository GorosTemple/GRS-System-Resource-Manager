// ==========================================================================
// Copyright © 2026 Marcos Eugenio Gorostegui
// Licencia de Uso No Comercial y Evaluación - Todos los derechos reservados.
//
// El código de cabecera y los archivos binarios asociados son propiedad
// exclusiva del autor. Se autoriza su uso únicamente con fines de
// evaluación y pruebas no comerciales. Su reproducción, distribución,
// modificación o uso comercial no autorizado está estrictamente prohibido.
//
// ESTE SOFTWARE SE PROPORCIONA "TAL CUAL", SIN GARANTÍA DE NINGÚN TIPO.
// ==========================================================================

#pragma once

#ifndef _GRS_DEPENDENCIES_
#define _GRS_DEPENDENCIES_
class GRS_DEPENDENCIES
{
    public: virtual ~GRS_DEPENDENCIES(void) = default;

    public:

        static GRS_DEPENDENCIES*AllocateMemory
        (
            const unsigned int allocatorType
        );

        static GRS_DEPENDENCIES*SetOutputType
        (
            const unsigned int outputType
        );
};
#endif  //  _GRS_DEPENDENCIES_
